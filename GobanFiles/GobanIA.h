#ifndef GOBANIA_H
#define GOBANIA_H

#include "Goban.h"
#include "../Partie/PartieIA.h"

class IA;

class GobanIA : public Goban//, boost::enable_shared_from_this<GobanIA>
{
public :
    GobanIA(double ecart, int size=9);
    bool coupPossible(int abs, int ord);
    boost::shared_ptr<PartieIA> getPartieIA() {return boost::dynamic_pointer_cast<PartieIA>(m_partie);}
    void copieGroupes(boost::shared_ptr<Goban> gobanPtr);
    void ajouterGroupe(boost::shared_ptr<Groupe> groupePtr);

    void init();
    boost::shared_ptr<GobanIA> sharedFromThis();

    //getters
    std::pair<int,int> getKo() const {return m_ko;}

    //gestion du ko -> vérification de la situation de ko, remise à (-1,-1) si nécessaire
    void gestionKo(std::vector<boost::shared_ptr<Pierre> > pierresCapturees);
    bool ko() const; //renvoie true s'il y a ko sur le goban sinon false

private :
//    boost::shared_ptr<PartieIA> m_partieIA;
    boost::shared_ptr<GobanIA> m_copie;
    std::pair<int,int> m_ko; //intersection sur laquelle il est interdit de jouer (situation de ko)
    //si (-1,-1) pas de situation de ko sur le goban
};

#endif // GOBANIA_H
