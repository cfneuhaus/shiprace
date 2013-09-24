#ifndef GERSTNERWATER_H
#define GERSTNERWATER_H

#include "Water.h"
#include <Eigen/Core>

#define AMPLITUDE_STEP 0.003
#define MAX_WAVE_NUM 3
#define GRAVITY 9.81

class GerstnerWater : public Water
{
public:
	GerstnerWater(int surfaceSize, double sizePerCell);
	virtual void update(double dt);

	//virtual double computeHeightAtXY(const Eigen::Vector2d& pos) const;
private:

	struct GerstnerWave
	{
		double f_time;           // aktueller Lebenszeitpunkt t der Welle
		double f_timeInterval;   // Größe des Zeitschritts der Welle

		double f_amplitude;      // aktuelle Amplitude a der Welle
		double f_maxAmplitude;   // maximale Amplitude einer Welle

		Eigen::Vector2d v_direction;  // Richtungsvektor D
		double f_wavelength;     // Wellenlänge l
		double f_phaseShift;     // Phasenverschiebung phi
		double f_phaseVelocity;  // Phasengeschwindigkeit c
		double f_frequency;      // Frequenz f
		double f_waveNumber;     // Wellenzahl k

		bool b_dying;           // Flaut die Welle schon ab?
	};

	void evalGerstner();
	Eigen::Vector3d evalGerstnerAtPoint(const Eigen::Vector2d& pos);

	void updateGerstner(double dt);
	void eraseGerstner(const std::vector<int>& del);
	void initNewWave();

	Eigen::Vector2d windDirection_;

	std::vector<GerstnerWave> waves_;

};

#endif
