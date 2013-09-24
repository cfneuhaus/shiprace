#include "GerstnerWater.h"
#include <iostream>


// TimeInterval:    z.B. 0.05, 0.8      bei 400x400 px
// Wavelength:      z.B. 5.0, 25.0      bei 400x400 px
// PhaseShift:      z.B. 0.0, 100.0     bei 400x400 px
inline double generateRandomFloat(double min, double max) {return (( (double) rand() / (double) RAND_MAX ) * ( max - min ) + min);}
// Direction:       z.B. 0.7, 0.7
//                  x und z geben eine Grundrichtung an
inline Eigen::Vector2d generateRandomVector(const Eigen::Vector2d& base)
{
	const Eigen::Vector2d pert=base+Eigen::Vector2d(( (double) rand() / ((double)RAND_MAX) ) / 2.0f,( (double) rand() / ((double)RAND_MAX) ) / 2.0f);
	return pert.normalized();
}

//-----------------------------------------------------------------------------
GerstnerWater::GerstnerWater(int surfaceSize, double sizePerCell) : Water(surfaceSize,sizePerCell)
{
}
//-----------------------------------------------------------------------------
//double GerstnerWater::computeHeightAtXY(const Eigen::Vector2d& pos) const
//{
//	//Eigen::Vector2d start=pos;
//}
//-----------------------------------------------------------------------------
void GerstnerWater::update(double dt)
{
	evalGerstner();
	updateGerstner(dt);
	updateGeometry();
}
//-----------------------------------------------------------------------------
Eigen::Vector3d GerstnerWater::evalGerstnerAtPoint(const Eigen::Vector2d& pos)
{
	double f_xSum = 0.0; double f_ySum = 0.0; double f_zSum = 0.0;

	// Iteriere über alle Wellen
	for (size_t w = 0; w < waves_.size(); w++)
	{
		const GerstnerWave& curWave=waves_[w];
		// Generiere Vertices für Gerstner Wellen nach folgenden Formeln:
		// x = x0 - SUMME( a * D.x * sin( dot( k*D, (x,z) ) - f*t*c + phi )
		// y =      SUMME( a       * cos( dot( k*D, (x,z) ) - f*t*c + phi )
		// z = z0 - SUMME( a * D.y * sin( dot( k*D, (x,z) ) - f*t*c + phi )
		float f_sincosArg = (curWave.f_waveNumber * curWave.v_direction).dot(pos) - curWave.f_frequency * curWave.f_time * curWave.f_phaseVelocity + curWave.f_phaseShift;

		f_xSum += curWave.f_amplitude * curWave.v_direction.x() * sin(f_sincosArg);
		f_ySum += curWave.f_amplitude * curWave.v_direction.y() * sin(f_sincosArg);
		f_zSum += curWave.f_amplitude * cos(f_sincosArg);
	}

	// Speicher den neuen Vertex
	return Eigen::Vector3d( (pos.x() - f_xSum) * sizePerCell_, (pos.y() - f_ySum) * sizePerCell_, f_zSum);
}
//-----------------------------------------------------------------------------
void GerstnerWater::evalGerstner()
{
	int i = 0;

	for (int y=(-1)*(surfaceSize_ / 2) + 1; y <= (surfaceSize_ / 2); y++)
		for (int x=(-1)*(surfaceSize_ / 2) + 1; x <= (surfaceSize_ / 2); x++)
			vertices_[i++]=evalGerstnerAtPoint(Eigen::Vector2d(x,y)).cast<float>();
}
//-----------------------------------------------------------------------------
void GerstnerWater::updateGerstner(double dt)
{
	std::vector<int> wavesToErase;

	// Iteriere über alle Wellen
	for (size_t w = 0; w < waves_.size(); w++)
	{
		GerstnerWave& curWave=waves_[w];

		// Abflauen der Welle einleiten
		if (curWave.f_amplitude > (curWave.f_maxAmplitude - AMPLITUDE_STEP))
			curWave.b_dying = true;

		// Welle wachsen lassen
		if ( curWave.b_dying == false )
			curWave.f_amplitude += AMPLITUDE_STEP;

		// Wellen abflauen lassen
		else
		{
			if (curWave.f_amplitude >= AMPLITUDE_STEP)
				curWave.f_amplitude -= AMPLITUDE_STEP;
			else
				wavesToErase.push_back(w);
		}

		curWave.f_time += curWave.f_timeInterval*dt;
	}
	// Abgeflaute Wellen löschen
	if (wavesToErase.size() > 0) eraseGerstner(wavesToErase);

	// ggf. neue Wellen generieren
	for (int i = waves_.size(); i < MAX_WAVE_NUM; i++)
		initNewWave();

	wavesToErase.empty();
}
//-----------------------------------------------------------------------------
void GerstnerWater::initNewWave()
{
	GerstnerWave wave;

	// Initialisiere Zeitverhalten
	wave.f_time = 0.0;
	wave.f_timeInterval = generateRandomFloat(0.05, 0.8);

	// Initialisiere Wellenparameter
	wave.f_amplitude = 0.0;

	wave.v_direction = generateRandomVector(windDirection_);
	wave.f_wavelength = generateRandomFloat(5, 25);

	wave.f_phaseShift = generateRandomFloat(0, 100);

	wave.f_phaseVelocity = sqrt((GRAVITY * wave.f_wavelength) / (2.0 * M_PI));
	wave.f_frequency = wave.f_phaseVelocity / wave.f_wavelength;
	wave.f_waveNumber = (2.0 * M_PI) / wave.f_wavelength;

	wave.f_maxAmplitude = (1.0f / wave.f_waveNumber) / (float)(MAX_WAVE_NUM * 1.5f);

	wave.b_dying = false;

	// Speicher die neue Welle im Vektor aller Wellen
	waves_.push_back(wave);

	// Wellen Info
	std::cout << "neue Welle Nr. " << waves_.size()-1 << " | Max: " << wave.f_maxAmplitude << " | Länge: " << wave.f_wavelength << " | Phase: " << wave.f_phaseShift << " | Zeitschritt: " << wave.f_timeInterval << std::endl;
}
//-----------------------------------------------------------------------------
void GerstnerWater::eraseGerstner(const std::vector<int>& del)
{
	// Lösche rückwärts alle abgeflauten Wellen aus dem Vektor aller Wellen
	for (int i = del.size() - 1; i >= 0; i --)
	{
		waves_.erase(waves_.begin() + del[i]);
	}
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
