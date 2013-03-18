#include "GobanIA.h"
#include "../IA/IA.h"
#include "../Tools/CoupException.h"

GobanIA::GobanIA(double ecart, int size) : Goban(ecart,size)
{
    boost::shared_ptr<PartieIA> partiePtr (new PartieIA());
    m_partie = partiePtr;
    m_ko = std::pair<int,int>(-1,-1);
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
            if (ko())
            {
                if ((m_ko.first==abs) && (m_ko.second==ord))
                    return false;
            }
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


/** Gestion du ko sur le goban : méthode appelée à la fin du tour d'un joueur (user ou ia)
  * Si ko!=(-1,-1) -> remettre le ko à (-1,-1)
  * Sinon :
  *     Si pierresCaptures.size()==1 (si on a capturé plus d'une pierre, on ne peut pas être dans une situation de ko)
  *         a = abscisse(pierreCapturée), o = ordonnée ; si jouer en (a,o) capture la pierre qui vient d'être posée,
  *         alors on est en situation de ko : il faut interdire pour le prochain tour de jouer en (a,o)
  */
void GobanIA::gestionKo(std::vector<boost::shared_ptr<Pierre> > pierresCapturees)
{
    if (m_ko.first!=-1)
    {
        m_ko.first=-1;
        m_ko.second = -1;
        koItem.reset();
    }
    else
    {
        if (pierresCapturees.size()==1)
        {
            int a = pierresCapturees.at(0)->getCoup().getAbs(), o = pierresCapturees.at(0)->getCoup().getOrd();
            Coup c (a,o,getPartieIA()->getJoueur(getPartieIA()->couleurAJouer()));
            boost::shared_ptr<Pierre> p;
            c.setNum(getPartieIA()->getListeCoups().size());
            p.reset(new Pierre(c,M_ECART));
            m_copie.reset(new GobanIA(5,9));
            m_copie->m_partie=m_partie;
            m_copie->copieGroupes(sharedFromThis());
            if ((m_copie->coupPossible(a,o)) && (m_copie->ajouterPierre(p,false).size()==1))
            {
                //alors il y a situation de ko !
                std::cout << "KO ! ! ! \n\n";
                m_ko.first = a; m_ko.second = o;
                QRect rect((a+1)*M_ECART-(M_ECART*0.31),(o+1)*M_ECART-(M_ECART*0.31),M_ECART*0.6,M_ECART*0.6);

                QPen blackPen(Qt::black,1.5);
                koItem.reset(this->addEllipse(rect,blackPen));
                koItem->setZValue(10);
            }
        }
    }
}


bool GobanIA::ko() const
{
    return (m_ko.first!=-1);
}
