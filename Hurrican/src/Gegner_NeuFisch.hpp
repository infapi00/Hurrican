#ifndef _GEGNER_NEUFISCH_HPP_
#define _GEGNER_NEUFISCH_HPP_

#include "GegnerClass.hpp"
#include "Gegner_Stuff.hpp"

class GegnerNeuFisch : public GegnerClass
{
private:
    PlayerClass*	pFest;
    float			MoveSpeed;
public :

    GegnerNeuFisch (int  Wert1, int Wert2,				// Konstruktor
                    bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
