#include "IA.h"
#include "../Partie/PartieIA.h"
#include "../GobanFiles/GobanIA.h"
#include "../Tools/CoupException.h"

#include <random>

IA::IA(boost::shared_ptr<PartieIA> partie, boost::shared_ptr<GobanIA> goban, std::string couleur)
    : Joueur("ia","ia",couleur), m_partie(partie), m_goban(goban)
{
//    std::cout << "IA créée\n";
}

std::pair<int,int> IA::choixCoup()
{
    std::random_device rd;
    int abs = -1, ord = -1;
    int i = 1;


    if (m_partie.lock()->partieFinie(m_goban.lock())) return std::pair<int,int>(abs,ord);

    while (!(m_goban.lock()->coupPossible(abs,ord)) && (i!=100))
    {
        std::cout << "Essai numéro " << i << "\n";

        abs = rd() % 9;
        ord = rd() % 9;

        std::cout << abs << "-" << ord << std::endl;

        i++;
    }

    if (i==100)
    {
        std::cout << "\n\nAUCUN COUP A JOUER POUR L'IA\n\n";
        abs = -1; ord = -1;
        //throw coup_exception("Impossible de trouver un coup à jouer pour l'IA\n");
    }

    return std::pair<int,int>(abs,ord);
}
