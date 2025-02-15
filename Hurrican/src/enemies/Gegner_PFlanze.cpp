// --------------------------------------------------------------------------------------
// Die Super-Mario Pflanze
//
// Spuckt Feuerbälle, wenn der Spieler in der Nähe ist
// --------------------------------------------------------------------------------------

#include "Gegner_Pflanze.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerPflanze::GegnerPflanze(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::LAUFEN;
    HitSound = 1;
    Energy = 80;
    AnimPhase = GetRandom(8);
    AnimEnde = 9;
    AnimSpeed = 2.2f - Skill * 0.4f;
    AnimCount = 0.0f;
    ChangeLight = Light;
    Destroyable = true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerPflanze::DoKI() {
    // Animieren
    //
    AnimCount += SpeedFaktor;   // Animationscounter weiterzählen
    if (AnimCount > AnimSpeed)  // Grenze überschritten ?
    {
        AnimCount = 0;  // Dann wieder auf Null setzen
        AnimPhase++;    // Und nächste Animationsphase

        if (AnimPhase == 14)  // Schuss abgeben
        {
            if (BlickRichtung == DirectionEnum::RECHTS)
                Projectiles.PushProjectile(xPos + 84.0f, yPos + 16.0f, PFLANZESHOT, pAim);
            else
                Projectiles.PushProjectile(xPos + 12.0f, yPos + 16.0f, PFLANZESHOT, pAim);
        }

        if (AnimPhase >= AnimEnde)  // Animation von zu Ende	?
        {
            // Schiessen ?
            //
            if (RunningTutorial == false && (PlayerAbstand() < 300 || GetRandom(2) == 0)) {
                Handlung = GEGNER::SCHIESSEN;
                AnimEnde = 17;
                AnimSpeed = 2.2f - Skill * 0.4f;
                AnimPhase = 9;
            }

            // oder normal wippen
            //
            else {
                Handlung = GEGNER::LAUFEN;
                AnimEnde = 8;
                AnimSpeed = 1.8f;
                AnimPhase = 0;
            }
        }
    }

    // In Richtung Spieler kucken
    //
    if (pAim->xpos + 35 < xPos + 55)
        BlickRichtung = DirectionEnum::LINKS;
    else
        BlickRichtung = DirectionEnum::RECHTS;
#if 0
    switch (Handlung) {
        // Nur rumwackeln
        //
        case GEGNER::LAUFEN:
            break;
    }
#endif
    // Testen, ob der Spieler die Pflanze berührt hat
    //
    TestDamagePlayers(Timer.sync(1.5f));
}

// --------------------------------------------------------------------------------------
// Pflanze explodiert
// --------------------------------------------------------------------------------------

void GegnerPflanze::GegnerExplode() {
    SoundManager.PlayWave(100, 128, 8000 + GetRandom(4000), SOUND::EXPLOSION1);

    for (int i = 0; i < 5; i++)
        PartikelSystem.PushPartikel(xPos + static_cast<float>(GetRandom(50)),
                                    yPos + static_cast<float>(GetRandom(40)), EXPLOSION_GREEN);

    for (int i = 0; i < 30; i++)
        // Blätter erzeugen
        PartikelSystem.PushPartikel(xPos + static_cast<float>(GetRandom(100)),
                                    yPos + 10.0f + static_cast<float>(GetRandom(50)), BLATT);

    Player[0].Score += 300;
}
