// --------------------------------------------------------------------------------------
// Die fette Spinne
//
// Krabbelt auf den Hurri zu und ballert Laser
// --------------------------------------------------------------------------------------

#include "Gegner_FetteSpinne.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerFetteSpinne::GegnerFetteSpinne(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::LAUFEN;
    Energy = 180;
    Value1 = Wert1;
    Value2 = Wert2;
    AnimStart = 0;
    AnimEnde = 9;
    AnimSpeed = 0.5f;
    ChangeLight = Light;
    Destroyable = true;
    ShotDelay = 10.0f;
    WalkState = 0;
    OwnDraw = true;
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerFetteSpinne::DoDraw() {

    // normal oder auf dem Kopf?
    bool v = (WalkState != 0);

    bool h = (BlickRichtung == DirectionEnum::RECHTS);

    pGegnerGrafix[GegnerArt]->itsRect = pGegnerGrafix[GegnerArt]->itsPreCalcedRects[AnimPhase];
    pGegnerGrafix[GegnerArt]->RenderMirroredSprite(xPos - TileEngine.XOffset,
                                                   yPos - TileEngine.YOffset, 0xFFFFFFFF, h, v);
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerFetteSpinne::DoKI() {
    // Animieren
    if (AnimEnde > 0)  // Soll überhaupt anmiert werden ?
    {
        AnimCount += SpeedFaktor;   // Animationscounter weiterzählen
        if (AnimCount > AnimSpeed)  // Grenze überschritten ?
        {
            AnimCount = 0;  // Dann wieder auf Null setzen
            AnimPhase++;    // Und nächste Animationsphase

            if (AnimPhase >= AnimEnde)  // Animation von zu Ende	?
                AnimPhase = AnimStart;  // Dann wieder von vorne beginnen
        }
    }  // animieren

    // Umdrehen wenn der Spieler von hinten drauf schiesst
    //
    if (DamageTaken > 0.0f) {
        if (pAim->xpos + 40.0f < xPos + 70.0f)
            BlickRichtung = DirectionEnum::LINKS;
        else
            BlickRichtung = DirectionEnum::RECHTS;
    }

    // Je nach Handlung richtig verhalten
    //
    switch (Handlung) {
        case GEGNER::LAUFEN: {
            // Speed setzen
            xAcc = 5.0f * Direction::asInt(BlickRichtung);

            xSpeed = std::clamp(xSpeed, -12.0f, 12.0f);

            // An Mauern umdrehen
            //
            if ((xSpeed < 0.0f && (blockl & BLOCKWERT_WAND || blockl & BLOCKWERT_GEGNERWAND)) ||
                (xSpeed > 0.0f && (blockr & BLOCKWERT_WAND || blockr & BLOCKWERT_GEGNERWAND)))
                BlickRichtung = Direction::invert(BlickRichtung);

            // Am Boden? Dann checken : Spieler schiesst auf die Spinne?
            if (WalkState == 0)
                if (((pAim->xpos + 35.0f < xPos + 50.0f &&
                    pAim->Blickrichtung == DirectionEnum::RECHTS && BlickRichtung == DirectionEnum::LINKS) ||
                    (pAim->xpos + 35.0f >= xPos + 50.0f &&
                    pAim->Blickrichtung == DirectionEnum::LINKS && BlickRichtung == DirectionEnum::RECHTS)) &&
                    pAim->Aktion[AKTION_SHOOT]) {
                    // Decke über der Spinne suchen
                    bool block = false;
                    int a = 0;
                    uint32_t b =
                        TileEngine.TileAt(static_cast<int>(xPos / TILESIZE_X),
                                          static_cast<int>(yPos / TILESIZE_Y) + a)
                            .Block;

                    while (a < 10 && block == false) {
                        b = TileEngine
                                .TileAt(static_cast<int>(xPos / TILESIZE_X),
                                        static_cast<int>(yPos / TILESIZE_Y) + a)
                                .Block;

                        // Decke gefunden?
                        if (b & BLOCKWERT_WAND)
                            block = true;

                        a++;
                    }

                    // Decke drüber? Dann hochspringen
                    if (block == true) {
                        Handlung = GEGNER::SPECIAL;
                        AnimPhase = 20;
                        AnimEnde = 0;
                        AnimSpeed = 0.0f;
                        AnimCount = 2.0f;
                        xSpeed = 0.0f;
                        xAcc = 0.0f;
                    }
                }

            // An der Decke? Dann auf den Spieler fallen lassen
            if (WalkState == 1) {
                if (yPos < pAim->ypos && PlayerAbstandHoriz() < 20) {
                    Handlung = GEGNER::SPECIAL;
                    AnimPhase = 20;
                    AnimEnde = 0;
                    AnimSpeed = 0.0f;
                    AnimCount = 0.0f;
                    xSpeed = 0.0f;
                    xAcc = 0.0f;
                }
            }

            // Zu weit vom Spieler weg? Dann umdrehen
            if (xPos + 50.0f < pAim->xpos + 35.0f - 200.0f)
                BlickRichtung = DirectionEnum::RECHTS;
            if (xPos + 50.0f > pAim->xpos + 35.0f + 200.0f)
                BlickRichtung = DirectionEnum::LINKS;

        } break;

        // kurz in die Hocke gehen
        case GEGNER::SPECIAL: {
            AnimCount -= Timer.sync(1.0f);

            // abspringen
            if (AnimCount < 0.0f) {
                Handlung = GEGNER::SPRINGEN;
                AnimPhase = 10;
                AnimEnde = 19;
                AnimSpeed = 0.5f;
                AnimStart = 19;

                if (WalkState == 0)
                    ySpeed = -25.0f;
                else {
                    ySpeed = 20.0f;
                    yAcc = 2.0f;
                }
            }
        } break;

        // im Flug drehen
        case GEGNER::SPRINGEN: {
            // An der Decke oder am Boden angekommen?
            if ((ySpeed < 0.0f && blocko & BLOCKWERT_WAND) || (ySpeed > 0.0f && blocku & BLOCKWERT_WAND) ||
                (ySpeed > 0.0f && blocku & BLOCKWERT_PLATTFORM)) {
                AnimPhase = 0;
                AnimStart = 0;
                AnimEnde = 9;
                AnimSpeed = 0.5f;

                ySpeed = 0.0f;
                yAcc = 0.0f;

                Handlung = GEGNER::LAUFEN;

                // umdrehen
                if (WalkState == 0)
                    WalkState = 1;
                else
                    WalkState = 0;
            }

        } break;

        default:
            break;

    }  // switch
}

// --------------------------------------------------------------------------------------
// Fette Spinne explodiert
// --------------------------------------------------------------------------------------

void GegnerFetteSpinne::GegnerExplode() {
    for (int i = 0; i < 15; i++)
        PartikelSystem.PushPartikel(xPos - 30.0f + static_cast<float>(GetRandom(170)),
                                    yPos - 30.0f + static_cast<float>(GetRandom(72)), EXPLOSION_MEDIUM2);

    for (int i = 0; i < 4; i++)
        PartikelSystem.PushPartikel(xPos + static_cast<float>(GetRandom(100)),
                                    yPos + static_cast<float>(GetRandom(30)), SPLITTER);

    for (int i = 0; i < 16; i++)
        PartikelSystem.PushPartikel(xPos + static_cast<float>(GetRandom(100)),
                                    yPos + static_cast<float>(GetRandom(30)), SPIDERSPLITTER);

    SoundManager.PlayWave(100, 128, 8000 + GetRandom(4000), SOUND::EXPLOSION3);

    Player[0].Score += 800;  // Punkte geben
}
