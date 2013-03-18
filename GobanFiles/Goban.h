#ifndef GOBAN_H
#define GOBAN_H

#include <QtGui>
#include <set>
#include <map>

#include "pierre.h"
#include "groupe.h"
#include "../Partie/Partie.h"

/** Classe qui représente le plateau : elle possède un ensemble de groupes de pierres
  *
  */

class Goban : public QGraphicsScene, public boost::enable_shared_from_this<Goban>
{
public :
    Goban(double ecart, int size=19);
    Goban(Goban const& g);

    //outils pour le jeu : ajout/suppression d'une pierre, etc
    std::vector<boost::shared_ptr<Pierre> > ajouterPierre(boost::shared_ptr<Pierre> p, bool trueGoban=true); //renvoie les pierres qui ont été supprimées du goban
    std::vector<boost::shared_ptr<Pierre> > pierresSansLibertes() const;
    std::set<boost::shared_ptr<Groupe> > groupesSansLiberte() const;
    std::vector<std::pair<int,int> > intersectionsAdjacentes(int abs, int ord);
    void supprimerPierre(boost::shared_ptr<Pierre> p);
    bool estSurPlateau(boost::shared_ptr<Pierre> p) const;

    // GETTERS
    //getPlateau : reconstitue une map représentant le plateau à partir du set de groupes
    std::map<std::pair<int,int>,boost::shared_ptr<Pierre> > getPlateau() const;
    int getCourant() {return m_courant;}
    std::set<boost::shared_ptr<Groupe> >& getGroupes() {return m_groupes;}

    //getters dessin
    QBrush getBrushFonce() const {return Goban::fondFonce;}
    QBrush getBrushMoyen() const {return fondMoyen;}
    QBrush getBrushClair() const {return fondClair;}
    QBrush getBrushSansMotif() const {return sansMotif;}
    static QPen getRouge();
    QGraphicsItemGroup* getLignes() const {return lignes;}
    boost::shared_ptr<QGraphicsEllipseItem> getCoupCourant() {return coupCourant;}
    boost::shared_ptr<QGraphicsEllipseItem> getKo() {return koItem;}


    ~Goban() {}



    void init();
    void avancer() {++m_courant;}
    void reculer() {--m_courant;}
    bool partieNonCommencee() const {return (m_courant==-1);}
    bool partieTerminee() const {return (m_courant!=-1) && (m_courant==m_partie->getListeCoups().size());}

    // SETTERS
    void setCourant(int num) {m_courant=num;}
    void setCoupCourant(boost::shared_ptr<QGraphicsEllipseItem> q) {coupCourant=q;}
    void setKo(boost::shared_ptr<QGraphicsEllipseItem> k) {koItem = k;}


    std::string printPlateau() const;

    unsigned int SIZE() {return M_SIZE;}
    double ECART() {return M_ECART;}

    boost::shared_ptr<Partie> getPartie() const {return m_partie;}
    void setPartie(boost::shared_ptr<Partie> p) {m_partie = p;}

protected :
    std::string m_name; //"SGF" ou "JEU" -> useless ?

    //outils pour dessiner la GraphicScene
    static QBrush noir;
    static QBrush blanc;
    static QPen pen;
    static QPen rouge;
    QGraphicsItemGroup* lignes;

    //couleurs de fond du goban, parce que c'est cool de pouvoir la changer
    QBrush fondClair;
    QBrush fondMoyen;
    QBrush fondFonce;
    QBrush sansMotif;


    //pour le jeu en lui-même :
    std::set<boost::shared_ptr<Groupe> > m_groupes; //ensemble des groupes présents sur le goban
    boost::shared_ptr<Partie> m_partie; //partie posée sur le plateau
    boost::shared_ptr<QGraphicsEllipseItem> coupCourant; //cercle rouge pour indiquer le dernier coup joué
    int m_courant; //indice du dernier coup joué, dans le vecteur de coups de la classe Partie
    boost::shared_ptr<QGraphicsEllipseItem> koItem; //indique un coup interdit à cause du ko



    QString logMsg; //useless, sûrement à supprimer

    unsigned int M_SIZE; //taille du goban : classiquement 9, 13 ou 19 (nombre de lignes)
    double M_ECART; //écart entre deux lignes (en pixels), utilisé pour calculer la taille des pierres par ex


};

#endif // GOBAN_H
