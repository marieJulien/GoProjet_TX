#include "goban.h"
//#include "../Tools/debug.h"
#include "../FenetrePrincipale/FenetrePrincipale.h"
#include "../Tools/CoupException.h"

QPen Goban::pen(Qt::black,1);
QPen Goban::rouge(Qt::red,1.5);
QBrush Goban::noir(Qt::black);
QBrush Goban::blanc(Qt::white);

using namespace std;

QPen Goban::getRouge()
{
    return rouge;
}

/** Constructeur de la classe Goban : construit la graphicScene � partir de size et ecart
  *
  */
Goban::Goban(double ecart, int size) : QGraphicsScene(), m_courant(-1), M_SIZE(size), M_ECART(ecart)
{
    //cr�ation d'une nouvelle partie par d�faut
    m_partie.reset(new Partie());

    ///CREATION DE LA GRAPHIC SCENE :
    //brush pour la couleur de fond
    fondClair = QBrush(QPixmap("Images/fondBoisClair.png").scaled(M_ECART*(M_SIZE+1),M_ECART*(M_SIZE+1),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    fondMoyen = QBrush(QPixmap("Images/fondBois.png").scaled(M_ECART*(M_SIZE+1),M_ECART*(M_SIZE+1),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    fondFonce = QBrush(QPixmap("Images/fondBoisFonce.png").scaled(M_ECART*(M_SIZE+1),M_ECART*(M_SIZE+1),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    sansMotif = QBrush(QColor(236,184,82));

    lignes = createItemGroup(QList<QGraphicsItem*>());

    for (unsigned int i = 0; i<M_SIZE ; i++)
    {
        lignes->addToGroup(addLine(M_ECART*(i+1),M_ECART,M_ECART*(i+1),M_SIZE*M_ECART));
        lignes->addToGroup(addLine(M_ECART,M_ECART*(i+1),M_SIZE*M_ECART,M_ECART*(i+1)));
    }
    //ajout des hoshi
    QPen penE(Qt::black,5);

    ///HOSHIS � placer en fonction de la taille du goban
    switch (M_SIZE)
    {
    case 19 :
    {
        for (unsigned int i = 0; i<3; i++)
        {
            lignes->addToGroup(addEllipse((M_ECART*4+(i*M_ECART*6))-1,M_ECART*4-1,2,2,penE,noir));
            lignes->addToGroup(addEllipse((M_ECART*4+(i*M_ECART*6))-1,M_ECART*10-1,2,2,penE,noir));
            lignes->addToGroup(addEllipse((M_ECART*4+(i*M_ECART*6))-1,M_ECART*16-1,2,2,penE,noir));
        }

        break;
    }

    case 13 :
    {
        lignes->addToGroup(addEllipse((M_ECART*4)-1,M_ECART*4-1,2,2,penE,noir));
        lignes->addToGroup(addEllipse((M_ECART*4)-1,M_ECART*10-1,2,2,penE,noir));
        lignes->addToGroup(addEllipse((M_ECART*10)-1,M_ECART*4-1,2,2,penE,noir));
        lignes->addToGroup(addEllipse((M_ECART*10)-1,M_ECART*10-1,2,2,penE,noir));
        lignes->addToGroup(addEllipse((M_ECART*7)-1,M_ECART*7-1,2,2,penE,noir));

        break;
    }

    case 9 :
    {
        lignes->addToGroup(addEllipse(M_ECART*3-1,M_ECART*3-1,2,2,penE,noir));
        lignes->addToGroup(addEllipse(M_ECART*3-1,M_ECART*7-1,2,2,penE,noir));
        lignes->addToGroup(addEllipse(M_ECART*7-1,M_ECART*3-1,2,2,penE,noir));
        lignes->addToGroup(addEllipse(M_ECART*7-1,M_ECART*7-1,2,2,penE,noir));
        lignes->addToGroup(addEllipse(M_ECART*5-1,M_ECART*5-1,2,2,penE,noir));

        break;
    }
    }

    setBackgroundBrush(fondClair);
}

Goban::Goban(Goban const& g)
{
    Goban::Goban(g.M_SIZE);
    coupCourant = g.coupCourant; m_courant=g.m_courant;
    koItem = g.koItem;
}

void Goban::init()
{
    map<pair<int,int>,boost::shared_ptr<Pierre> > plateau = getPlateau();
    for (map<pair<int,int>,boost::shared_ptr<Pierre> >::iterator it = plateau.begin(); it!=plateau.end(); ++it)
    {
        removeItem((*it).second->getEllipse().get());
    }
    coupCourant.reset();
    m_courant = -1;
    m_groupes.clear();
}


/** Ajout de la pierre p au plateau
  * le bool�en trueGoban sert � v�rifier qu'on ajoute sur la pierre sur le plateau r�el
  * ou sur une copie de plateau (pour tester qu'un coup est possible par exemple)
  *
  * On part du principe que le coup est possible (v�rification � faire AVANT d'appeler cette m�thode).
  * Dans un premier temps on va chercher � fusionner �ventuellement des groupes de pierres :
  *     On r�cup�re les pierres autour de la nouvelle pierre pos�e, on regarde leur couleur et le groupe
  *     auquel elles appartiennent, et on fusionne si n�cessaire.
  *     S'il n'y a aucune pierre de la m�me couleur autour de p, on doit cr�er un nouveau groupe qui contient p.
  *
  * Dans un deuxi�me temps on regarde si on a captur� des pierres adverses en posant p.
  */
vector<boost::shared_ptr<Pierre> > Goban::ajouterPierre(boost::shared_ptr<Pierre> p, bool trueGoban)
{
    std::cout << "\n\n- - - - - - - Debut ajouter pierre (p)\n\n";
    const int ord = p->getCoup().getOrd();
    const int abs = p->getCoup().getAbs();

    /**************************************** FUSION DES GROUPES ***********************************/

    vector<boost::shared_ptr<Pierre> > pierresAutour = p->pierresAutourMemeCouleur(shared_from_this());
    cout << pierresAutour.size() << " pierres autour de la m�me couleur\n";
    vector<boost::shared_ptr<Groupe> > groupesAutour;

    //on r�cup�re les groupes adjacents � p dans le vector groupesAutour : pour chaque pierre autour de p, on regarde
    //� quel groupe elle appartient
    for (vector<boost::shared_ptr<Pierre> >::iterator it = pierresAutour.begin(); it != pierresAutour.end(); it++)
    {
        boost::shared_ptr<Groupe> m_groupe = (*it)->getGroupe();
        if (find(groupesAutour.begin(), groupesAutour.end(), m_groupe)==groupesAutour.end())
        {
            groupesAutour.push_back(m_groupe);
        }
    }

    cout << groupesAutour.size() << " groupes autour de la m�me couleur\n";

    if (groupesAutour.size()==0)
    {
        //aucun groupe autour, il faut en cr�er un nouveau
        boost::shared_ptr<Groupe> groupePtr(new Groupe());
        groupePtr->ajouterPierre(p);
        p->setGroupe(groupePtr->shared_from_this());

        //on ajoute le nouveau groupe � m_groupes
        m_groupes.insert(groupePtr);
        groupePtr->setGoban(shared_from_this());
        std::cout << "ajout au nouveau groupe ok" << std::endl;
    }
    else
    {
        /*il y a plusieurs groupes � fusionner : on prend le premier, on lui ajoute les autres
          un par un, puis on ajoute la nouvelle pierre cr��e */
        boost::shared_ptr<Groupe> groupePtr = groupesAutour[0];
        for (vector<boost::shared_ptr<Groupe> >::iterator it = groupesAutour.begin(); it != groupesAutour.end(); it++)
        {
            if (it!=groupesAutour.begin())
            {
                groupePtr->ajouterGroupe(*it);
                m_groupes.erase(*it);
                cout << "suppression de l'ancien groupe\n";
            }

        }

        groupePtr->ajouterPierre(p);
        p->setGroupe(groupePtr->shared_from_this());
    }

    /****************************************** Affichage ******************************************/
    if (trueGoban)
    {
        addItem(p->getEllipse().get());
        QRect rect((abs+1)*M_ECART-(M_ECART*0.31),(ord+1)*M_ECART-(M_ECART*0.31),M_ECART*0.6,M_ECART*0.6);

        coupCourant.reset(this->addEllipse(rect,rouge));
        coupCourant->setZValue(10);
        std::cout << "Ajout du coup courant\n";
    }
    //si le goban sur lequel on ajoute p n'est pas un "vrai goban" (si c'est une copie r�alis�e pour tester),
    //on ne va pas chercher � afficher la pierre � l'�cran


    /************************* Suppression des pierres *********************************************/
    vector<boost::shared_ptr<Pierre> > pierresSupprimees;
    set<boost::shared_ptr<Groupe> > sansLibertes = groupesSansLiberte();
    //on r�cup�re toutes les cha�nes sans libert� pr�sentes sur le plateau

    if (sansLibertes.size()>0)
    {
        std::ostringstream os;
        os << sansLibertes.size() << " groupes sans libert� sur le plateau";
        for (set<boost::shared_ptr<Groupe> >::iterator it = sansLibertes.begin() ; it != sansLibertes.end() ; ++it)
        {
            boost::shared_ptr<Groupe> groupePtr = *it;
            //si groupe adversaire, alors on supprime toutes ses pierres une par une, puis on supprime le groupe
            if (groupePtr->couleur()!=p->couleur())
            {
                vector<boost::shared_ptr<Pierre> > pierres = groupePtr->getPierres();
                for (vector<boost::shared_ptr<Pierre> >::iterator it_pierre = pierres.begin(); it_pierre != pierres.end(); it_pierre++)
                {
                    //pour chaque pierre du groupe � supprimer
                    pierresSupprimees.push_back(*it_pierre);
                    int abs, ord;
                    abs = (*it_pierre)->getCoup().getAbs(); ord = (*it_pierre)->getCoup().getOrd();
                    supprimerPierre(*it_pierre);
                }
                m_groupes.erase(groupePtr);
            }
        }
    }

    int ajoutLibertes = p->getGroupe()->nbLibertes();
    std::cout << "nombre de libert�s du groupe de la pierre ajout�e : " << ajoutLibertes << std::endl;
    if (ajoutLibertes==0)
        throw coupImpossible("aucune libert�");

    std::cout << " - - - - - - - end ajouter pierre(p)\n\n";
    if (p->getGroupe().get()==0)
    {
        throw coup_exception("La pierre n'appartient � aucun groupe !!");
    }
    return pierresSupprimees;
}





void Goban::supprimerPierre(boost::shared_ptr<Pierre> p)
{
    removeItem(p->getEllipse().get());
}



bool Goban::estSurPlateau(boost::shared_ptr<Pierre> p) const
{
    int abs = p->getCoup().getAbs(), ord = p->getCoup().getOrd();
    map<pair<int,int>,boost::shared_ptr<Pierre> > plateau = getPlateau();
    return (plateau.find(pair<int,int>(abs,ord))!=plateau.end());
}

vector<boost::shared_ptr<Pierre> > Goban::pierresSansLibertes() const
{
    vector<boost::shared_ptr<Pierre> > result;
    map<pair<int,int>,boost::shared_ptr<Pierre> > plateau = getPlateau();

    for (map<pair<int,int>,boost::shared_ptr<Pierre> >::iterator it = plateau.begin(); it !=plateau.end(); ++it)
    {
        if (it->second->libertes().size()==0)
        {
            result.push_back(it->second);
        }
    }
    return result;
}

set<boost::shared_ptr<Groupe> > Goban::groupesSansLiberte() const
{
    set<boost::shared_ptr<Groupe> > result;
    for (set<boost::shared_ptr<Groupe> >::iterator it = m_groupes.begin(); it != m_groupes.end(); it++)
    {
        if ((*it)->nbLibertes()==0)
        {
            result.insert(*it);
        }
    }
    return result;
}


//affichage pour du debug, mais le r�sultat est juste d�gueu
std::string Goban::printPlateau() const
{
    std::ostringstream result;
    result << "Etat du plateau : \n";
    map<pair<int,int>,boost::shared_ptr<Pierre> > plateau = getPlateau();
    for (map<pair<int,int>,boost::shared_ptr<Pierre> >::const_iterator it = plateau.begin(); it != plateau.end(); ++it)
    {
        // result << "Pierre " << it->second->getCoup().getJoueur()->couleur().toStdString() << " en " << it->first.first << "-" << it->first.second << "\n";
        result << "Pierre " << it->second->getCoup().print() << "\n";

    }

    for (std::set<boost::shared_ptr<Groupe> >::iterator it = m_groupes.begin(); it != m_groupes.end(); it++)
    {
        result << "Groupe : " << (*it)->printToString() << std::endl;
    }

    result << "\n\n";
    return result.str();
}

std::map<std::pair<int,int>,boost::shared_ptr<Pierre> >  Goban::getPlateau() const
{
    std::map<std::pair<int,int>,boost::shared_ptr<Pierre> > result;

    for(std::set<boost::shared_ptr<Groupe> >::iterator it = m_groupes.begin(); it != m_groupes.end(); it++)
    {
        boost::shared_ptr<Groupe> groupePtr = *it;
        std::vector<boost::shared_ptr<Pierre> > pierres = groupePtr->getPierres();

        for (std::vector<boost::shared_ptr<Pierre> >::iterator itp = pierres.begin(); itp != pierres.end(); itp++)
        {
            boost::shared_ptr<Pierre> pierrePtr = *itp;
            int abs = pierrePtr->getCoup().getAbs();
            int ord = pierrePtr->getCoup().getOrd();

            result.insert(std::pair<std::pair<int,int>,boost::shared_ptr<Pierre> >(std::pair<int,int>(abs,ord),pierrePtr));
        }
    }

    return result;
}


//renvoie un vector de pair<abscisse, ordonn�e> qui repr�sente les intersections adjacentes � une intersection donn�e
//d�pend de la taille du goban M_SIZE
std::vector<std::pair<int,int> > Goban::intersectionsAdjacentes(int abs, int ord)
{
    vector<pair<int, int> > resultat;

    resultat.push_back(pair<int,int>(abs,ord+1));
    resultat.push_back(pair<int,int>(abs,ord-1));
    resultat.push_back(pair<int,int>(abs+1,ord));
    resultat.push_back(pair<int,int>(abs-1,ord));

    for (vector<pair<int,int> >::iterator it = resultat.begin(); it != resultat.end();)
    {
        int a = it->first; int o = it->second;
        if (a<0 || a>=SIZE())
        {
            it = resultat.erase(it);
        }
        else if (o<0 || o>=SIZE())
        {
            it = resultat.erase(it);
        }
        else it++;
    }

    return resultat;
}
