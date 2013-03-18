#include "GobanIA.h"
#include "../IA/IA.h"
#include "../Tools/CoupException.h"

GobanIA::GobanIA(double ecart, int size) : Goban(ecart,size)
{
    boost::shared_ptr<PartieIA> partiePtr (new PartieIA());
    m_partie = partiePtr;
}

void GobanIA::init()
{
    try
    {
        boost::shared_ptr<PartieIA> partiePtr = boost::dynamic_pointer_cast<PartieIA>(m_partie);
        if (partiePtr.get()==0) throw coup_exception("\n!!!!!! partie NULL !!!!!\n"); ///ERREUR
        boost::shared_ptr<GobanIA> shrdThis = sharedFromThis();
        if (shrdThis.get()!=0)
            partiePtr->init(shrdThis);
        else throw coup_exception("sharedFromThis de GobanIA = NULL");
    }
    catch(std::exception const& e)
    {
        std::cout << e.what();
        throw std::exception(e);
    }
}

bool GobanIA::coupPossible(int abs, int ord)
{
    std::cout << "GobanIA::CoupPossible\n";
    try
    {
        std::pair<int,int> coord (abs,ord);
        std::map<std::pair<int,int>,boost::shared_ptr<Pierre> > plateau = getPlateau();
        std::cout << "getPlateau : done\n";

        if ((plateau.find(coord)!=plateau.end()) || (abs < 0) || (abs > 9) || (ord < 0) || (ord > 9))
        {
            return false;
        }
        else
        {
            boost::shared_ptr<Pierre> p;
            if ("noir"==getPartieIA()->couleurAJouer())
            {
                if (m_partie->getNoir().get()==0) std::cout << "ERREUR : POINTEUR SUR NOIR DANS PARTIE = 0\n\n";
                Coup c(abs,ord, m_partie->getNoir());
                std::cout << "get noir : done\n";
                c.setNum(getPartieIA()->getListeCoups().size());
                std::cout << "get partie IA : done\n";
                p.reset(new Pierre(c,M_ECART));
            }
            else
            {
                Coup c(abs,ord, m_partie->getBlanc());
                c.setNum(getPartieIA()->getListeCoups().size());
                p.reset(new Pierre(c,M_ECART));
            }
            std::cout << "AAAAAAAAAAAAAA\n";
            m_copie.reset(new GobanIA(5,9));
            std::cout << "m_copie.reset : done\n";
            m_copie->m_partie=m_partie;
            m_copie->copieGroupes(sharedFromThis());
            std::cout << "copie groupes : ok\n";

            try
            {
                m_copie->ajouterPierre(p,false);
            }
            catch(coupImpossible& e)
            {
                std::cout << "Coup impossible\n";
                return false;
            }

            return true;


        }

        ///TODO à terminer
    }
    catch(std::exception const& e)
    {
        std::ostringstream errorMsg;
        errorMsg << "Impossible de déterminer si le coup " << abs << " - " << ord << " est possible : \n " << e.what();
        throw coup_exception(errorMsg.str());
    }
}

boost::shared_ptr<GobanIA> GobanIA::sharedFromThis()
{
    try
    {
        return boost::shared_dynamic_cast<GobanIA,Goban>(shared_from_this());
    }
    catch(std::exception const& e)
    {
        std::cout << "Impossible de créer un shared ptr à partir de GobanIA : " << e.what();
        throw std::exception(e);
    }
}

void GobanIA::copieGroupes(boost::shared_ptr<Goban> gobanPtr)
{
    for(std::set<boost::shared_ptr<Groupe> >::iterator it = gobanPtr->getGroupes().begin(); it != gobanPtr->getGroupes().end(); it++)
    {
        boost::shared_ptr<Groupe> groupePtr (new Groupe(*it));
        ajouterGroupe(groupePtr);
        groupePtr->copyPierres(*it);
    }
}

void GobanIA::ajouterGroupe(boost::shared_ptr<Groupe> groupePtr)
{
    m_groupes.insert(groupePtr);
    groupePtr->setGoban(sharedFromThis());
}
