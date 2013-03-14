#include "PartieIA.h"
#include "Partie.h"
#include "../IA/IA.h"
#include "../IA/User.h"
#include "Joueur.h"
#include "../GobanFiles/GobanIA.h"
#include "../Tools/CoupException.h"

PartieIA::PartieIA(std::string couleurIA,int handicap,double komi)
    : Partie(), m_couleurIA(couleurIA), m_handicap(handicap), m_komi(komi)
{
}

void PartieIA::init(boost::shared_ptr<GobanIA> gobanPtr)
{
//    std::cout << "init partie ia\n";

    try
    {
        if (m_couleurIA=="noir")
        {
            m_joueurNoir.reset(new IA(sharedFromThis(),gobanPtr,"noir"));
            m_joueurBlanc.reset(new User(sharedFromThis(),gobanPtr,"blanc"));
        }
        else
        {

            m_joueurBlanc.reset(new IA(sharedFromThis(),gobanPtr,"blanc"));
            m_joueurNoir.reset(new User(sharedFromThis(),gobanPtr,"noir"));
        }
    }
    catch(std::exception e)
    {
        std::cout << "Impossible d'initialiser la partieIA : " << e.what();
    }
}

boost::shared_ptr<IA> PartieIA::getIA()
{
    if (m_couleurIA=="noir")
    {
        return boost::dynamic_pointer_cast<IA>(m_joueurNoir);
    }
    else return boost::dynamic_pointer_cast<IA>(m_joueurBlanc);
}

boost::shared_ptr<User> PartieIA::getUser()
{
    if (m_couleurIA=="noir")
    {
        return boost::dynamic_pointer_cast<User>(m_joueurBlanc);
    }
    else return boost::dynamic_pointer_cast<User>(m_joueurNoir);
}


std::string PartieIA::couleurAJouer()
{
    std::cout << "couleur a jouer, nb coups = " << m_coups.size() << std::endl;
    if (m_coups.size()%2==0)
    {
        //un nombre pair de coups a été joué
        if (m_handicap==0)
        {
            return "noir";
        }
        else return "blanc";
    }
    else
    {
        if (m_handicap==0)
        {
            return "blanc";
        }
        else return "noir";
    }
}


boost::shared_ptr<PartieIA> PartieIA::sharedFromThis()
{
    return boost::shared_dynamic_cast<PartieIA,Partie>(shared_from_this());
}

bool PartieIA::partieFinie(boost::shared_ptr<GobanIA> gobanPtr)
{
    //std::cout << "partie finie ?\n"; return false;
    if (gobanPtr->getGroupes().size()==0) return false;
    if (m_coups.size() >= 2)
        if ((m_coups.at(m_coups.size()-1).getAbs()==-1) && (m_coups.at(m_coups.size()-2).getAbs()==-1))
            return true;

    for(std::set<boost::shared_ptr<Groupe> >::iterator it = gobanPtr->getGroupes().begin(); it != gobanPtr->getGroupes().end(); it++)
    {
        boost::shared_ptr<Groupe> groupePtr = *it;
        if (groupePtr->nbLibertes()!=2)
        {
            return false;
        }
    }

    return true;

}


std::string PartieIA::resultat(boost::shared_ptr<GobanIA> goban)
{
    double pointsIA = 0, pointsUser = 0;
    boost::shared_ptr<IA> IA = getIA();
    boost::shared_ptr<User> User = getUser();

    for(std::set<boost::shared_ptr<Groupe> >::iterator it = goban->getGroupes().begin(); it != goban->getGroupes().end(); it++)
    {
        boost::shared_ptr<Groupe> groupePtr = *it;
        int libertesInternes = groupePtr->nbLibertesInternes();
        if (groupePtr->couleur()==IA->couleur())
        {
            pointsIA += libertesInternes;
        }
        else if (groupePtr->couleur()==User->couleur())
        {
            pointsUser += libertesInternes;
        }
    }

    double terrUser = pointsUser, terrIA = pointsIA;

    pointsIA += IA->getCapt();
    pointsUser += User->getCapt();

    if (IA->couleur()=="blanc") pointsIA += m_komi;
    else pointsUser += m_komi;

    std::ostringstream resultat;
    resultat << "Fin de la partie : ";
    if (pointsIA > pointsUser) resultat << "victoire de l'IA (" << IA->couleur() << ") de " << (pointsIA - pointsUser) << std::endl;
    else resultat << "victoire de l'utilisateur (" << User->couleur() << ") de " << (pointsUser - pointsIA) << std::endl;
    resultat << "Utilisateur (" << User->couleur() << ") : "
             << pointsUser << " points (territoire : "
             << terrUser << ", prisonniers : " << User->getCapt();
    if (User->couleur()=="blanc") resultat << ", komi : " << m_komi;
    resultat << ")" << std::endl;
    resultat << "IA (" << IA->couleur() << ") : "
             << pointsIA << " points (territoire : "
             << terrIA << ", prisonniers : " << IA->getCapt();
    if (IA->couleur()=="blanc") resultat << ", komi : " << m_komi;
    resultat << ")" << std::endl;

    return resultat.str();
}
