#ifndef _TRIGGER_SCHLEUSEV_HPP_
#define _TRIGGER_SCHLEUSEV_HPP_

#include "GegnerClass.hpp"
#include "Gegner_Stuff.hpp"

class GegnerSchleuseV : public GegnerClass
{
public:

    GegnerSchleuseV  (int Wert1,	int Wert2,		// Konstruktor
                      bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif