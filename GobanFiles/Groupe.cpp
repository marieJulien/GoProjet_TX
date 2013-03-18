#include "groupe.h"
#include "pierre.h"
#include "Goban.h"
#include "../Tools/CoupException.h"

using namespace std;

Groupe::Groupe() : statut(0)
{}


unsigned int Groupe::nbLibertes() const
{
    //unsigned int result=0;
    set<pair<int,int> > libertes;


    for (vector<boost::shared_ptr<Pierre> >::const_iterator it = m_pierres.begin(); it != m_pierres.end() ; ++it)
    {
        std::vector<pair<int,int> > libertesPierre = (*it)->libertes();
        for(std::vector<pair<int,int> >::iterator it_l = libertesPierre.begin(); it_l != libertesPierre.end(); it_l++)
        {
            if (libertes.find(*it_l)==libertes.end())
                libertes.insert(*it_l);
        }
    }

    return libertes.size();
}

unsigned int Groupe::nbLibertesInternes() const
{
    return libertesInternes().size();
}


//renvoie la liste des libertés internes du groupe
std::set<pair<int,int> > Groupe::libertesInternes() const
{
    //unsigned int result=0;
    set<pair<int,int> > libertes;
    map<pair<int,int>, boost::shared_ptr<Pierre> > plateau = m_goban.lock()->getPlateau();

    //pour chaque pierre du groupe
    for (vector<boost::shared_ptr<Pierre> >::const_iterator it = m_pierres.begin(); it != m_pierres.end() ; ++it)
    {
        std::vector<pair<int,int> > libertesPierre = (*it)->libertes();

        //pour chaque liberté de la pierre
        for(std::vector<pair<int,int> >::iterator it_l = libertesPierre.begin(); it_l != libertesPierre.end(); it_l++)
        {
            //si on a pas déjà regardé cette liberté
            if (libertes.find(*it_l)==libertes.end())
            {
                int abs = it_l->first, ord = it_l->second;
                vector<pair<int,int> > adj = m_goban.lock()->intersectionsAdjacentes(abs,ord);
                bool interne = true;
                for(vector<pair<int,int> >::iterator it_adj = adj.begin(); it_adj != adj.end(); it_adj++)
                {
                    if (plateau.find(*it_adj)!=plateau.end())
                    {
                        //il y a une pierre à côté de l'intersection
                        boost::shared_ptr<Pierre> pierrePtr = (*(plateau.find(*it_adj))).second;
                        if (pierrePtr->couleur()!=couleur())
                        {
                            //cette pierre appartient à l'adversaire -> pas une liberté interne
                            interne = false;
                        }
                    }
                }

                //si cette liberté est une liberté interne, on l'ajoute au résultat
                if (interne)
                    libertes.insert(*it_l);
            }
        }
    }

    return libertes;
}

bool Groupe::faitPartie(const boost::shared_ptr<Pierre> p) const
{
    return (find(m_pierres.begin(), m_pierres.end(),p)!=m_pierres.end());
}

void Groupe::ajouterPierre(boost::shared_ptr<Pierre> p)
{
    if (faitPartie(p))
    {
        ostringstream res;
        res << "La pierre " << p->getCoup().getAbs() << "-" << p->getCoup().getOrd() << " appartient déjà au groupe";
        throw coup_exception(res.str());
    }
    m_pierres.push_back(p);
    p->setGroupe(this->shared_from_this());
}

void Groupe::ajouterGroupe(boost::shared_ptr<Groupe> g)
{
    if (g.get()!=0)
    {
        int i = 1;
        for (vector<boost::shared_ptr<Pierre> >::iterator it = g->getPierres().begin(); it != g->getPierres().end(); it++)
        {
            boost::shared_ptr<Pierre> pierrePtr = *it;
            ajouterPierre(pierrePtr);
            pierrePtr->setGroupe(this->shared_from_this());
            i++;
        }
    }
    else
    {
        cout << "Pointeur sur groupe à ajouter nul" << endl;
    }
}

void Groupe::capture()
{}




Groupe::Groupe(Groupe const& g, bool copyPtr)
{
    statut = g.statut;
    for (vector<boost::shared_ptr<Pierre> >::const_iterator it=g.m_pierres.begin(); it!=g.m_pierres.end() ; ++it)
    {
        //copie de la pierre
        boost::shared_ptr<Pierre> newPierre (new Pierre(*((*it).get()),copyPtr));
        m_pierres.push_back(newPierre);
        newPierre->setGroupe(shared_from_this());
    }
}

void Groupe::print() const
{
    if (m_pierres.size()!=0)
    {
        for (vector<boost::shared_ptr<Pierre> >::const_iterator it=m_pierres.begin(); it!=m_pierres.end(); ++it)
            std::cout << (*it)->getCoup().getAbs() <<"-" << (*it)->getCoup().getOrd() <<" + ";
    }
    else std::cout << "Groupe vide";
}

std::string Groupe::printToString() const
{
    std::ostringstream os;

    if (m_pierres.size()!=0)
    {
        os << couleur() << " - ";
        for (vector<boost::shared_ptr<Pierre> >::const_iterator it=m_pierres.begin(); it!=m_pierres.end(); ++it)
            os << (*it)->getCoup().getAbs() <<"-" << (*it)->getCoup().getOrd() <<" + ";
    }
    else os << "Groupe vide";

    return os.str();
}

std::string Groupe::couleur() const
{
    if (!m_pierres.empty())
    {
        return m_pierres[0]->couleur();
    }
}

boost::shared_ptr<Goban> Groupe::getGoban() const
{
    return m_goban.lock();
}

void Groupe::setGoban(boost::shared_ptr<Goban> gobanPtr)
{
    m_goban = boost::weak_ptr<Goban>(gobanPtr);
}

Groupe::Groupe(boost::shared_ptr<Groupe> groupePtr)
{
    statut = groupePtr->statut;
    if (groupePtr.get()==0) throw coup_exception("GroupePtr=NULL\n");

}


void Groupe::copyPierres(boost::shared_ptr<Groupe> groupePtr, bool copyPtr)
{
    for (vector<boost::shared_ptr<Pierre> >::const_iterator it=groupePtr->m_pierres.begin(); it!=groupePtr->m_pierres.end() ; ++it)
    {
        //copie de la pierre
        boost::shared_ptr<Pierre> newPierre (new Pierre(*it,copyPtr));
        m_pierres.push_back(newPierre);
        newPierre->setGroupe(shared_from_this());
    }
}
