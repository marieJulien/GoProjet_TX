#include "FenetreJeu.h"
#include "DebutJeu.h"
#include "../GobanFiles/GobanIA.h"
#include "../IA/IA.h"
#include "../Tools/CoupException.h"

FenetreJeu::FenetreJeu() : FenetrePrincipale()
{
//    std::cout << "Création fenêtre de jeu\n";
    QDesktopWidget desk;
    qreal m_height = desk.height();
    ECART = ceil((m_height-150)/(10));
    m_goban.reset(new GobanIA(ECART,9));
    //    m_goban->init();

    vue = new QGraphicsView(m_goban.get());
    vue->setFixedSize(ECART*(m_goban->SIZE()+1),ECART*(m_goban->SIZE()+1));
    layoutV->addWidget(vue);


    /********* Définition d'une grille de boutons correspondant aux intersections *********/
    grilleBoutonsGoban =new QGridLayout();
    for (unsigned int i = 0; i<m_goban->SIZE(); i++)
        for (unsigned int j = 0; j<m_goban->SIZE(); j++)
        {
            BoutonGoban* bouton =new BoutonGoban(i,j,m_goban);
            grilleBoutonsGoban->addWidget(bouton,j+1,i+1);
            connect(bouton,SIGNAL(clicked()),bouton,SLOT(envoyerSignalClicked()));
            connect(bouton,SIGNAL(clickedBouton(int,int)),this,SLOT(bouton_goban(int,int)));
        }
    grilleBoutonsGoban->setSpacing(0);
    grilleBoutonsGoban->setMargin(ECART/2);
    vue->setLayout(grilleBoutonsGoban);


    /*Quand on met une pierre sur un bord pour la première fois, le goban se décale ... En attendant d'avoir
    réglé le problème, on met des pierres dans les coins pour que le goban soit à la bonne
    place*/
    QGraphicsPixmapItem* ellipse = new QGraphicsPixmapItem(QPixmap("Images/pierreNoire.png").scaled(ECART*R,ECART*R,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    ellipse->setX(ECART-(ECART*R/2));
    ellipse->setY(ECART-(ECART*R/2));

    ellipse->setVisible(false);
    m_goban->addItem(ellipse);

    QGraphicsPixmapItem* ellipse2 = new QGraphicsPixmapItem(QPixmap("Images/pierreNoire.png").scaled(ECART*R,ECART*R,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    ellipse2->setX(m_goban->SIZE()*ECART-(ECART*R/2));
    ellipse2->setY(m_goban->SIZE()*ECART-(ECART*R/2));

    ellipse2->setVisible(false);
    m_goban->addItem(ellipse2);



    ///BOUTONS
    layoutBoutonsNP =new QHBoxLayout();

    resignButton = new QPushButton("Abandon");
    passButton = new QPushButton("Passer");
    layoutBoutonsNP->addWidget(passButton);
    layoutBoutonsNP->addWidget(resignButton);

    layoutBoutons = new QVBoxLayout();
    layoutV->addLayout(layoutBoutons);
    layoutBoutons->addLayout(layoutBoutonsNP);

    connect(resignButton,SIGNAL(clicked()),this,SLOT(abandon()));
    connect(passButton,SIGNAL(clicked()),this,SLOT(passer()));
}


void FenetreJeu::bouton_goban(int a, int o)
{
    try
    {
        std::cout << "Clicked : " << a << "-" << o << std::endl;
        boost::shared_ptr<GobanIA> gobanPtr = boost::dynamic_pointer_cast<GobanIA>(m_goban);

        if (!gobanPtr->getPartieIA()->partieFinie())
        {
            if (gobanPtr->getPartieIA()->couleurAJouer()!=gobanPtr->getPartieIA()->getCouleurIA())
            {
                //si c'est bien à l'utilisateur de jouer
                if (gobanPtr->coupPossible(a,o))
                {
                    std::cout << "\ncoup utilisateur possible\n";
                    boost::shared_ptr<Joueur> joueurUser = gobanPtr->getPartieIA()->getJoueur(gobanPtr->getPartieIA()->couleurAJouer());
                    Coup c(a,o,joueurUser);
                    c.setNum(gobanPtr->getPartieIA()->getListeCoups().size());
                    gobanPtr->getPartieIA()->ajouterCoup(c);
                    boost::shared_ptr<Pierre> pierrePtr(new Pierre(c,gobanPtr->ECART()));
                    gobanPtr->ajouterPierre(pierrePtr);

                    std::cout << "coup de l'utilisateur : ok, maintenant choix de l'ia\n";

                    //ensuite, l'IA doit choisir un coup
                    std::pair<int,int> coupIA = gobanPtr->getPartieIA()->getIA()->choixCoup();
                    std::cout << "Choix de l'ia : " << coupIA.first << " - " << coupIA.second << std::endl;
                    Coup c2(coupIA.first,coupIA.second,gobanPtr->getPartieIA()->getIA());
                    c2.setNum(gobanPtr->getPartieIA()->getListeCoups().size());
                    gobanPtr->getPartieIA()->ajouterCoup(c2);
                    boost::shared_ptr<Pierre> pierre2 (new Pierre(c2,gobanPtr->ECART()));
                    gobanPtr->ajouterPierre(pierre2);
                }
                else
                {
                    std::cout << "!!!!! COUP IMPOSSIBLE !!!!!\n";
                    ///TODO afficher une fenêtre d'erreur "coup non valide". fenêtre modale ?
                }
            }
        }
    }
    catch(std::exception const& e)
    {
        std::ostringstream errorMsg;
        errorMsg << "Erreur après clic de l'utilisateur : \n " << e.what();
        throw coup_exception(errorMsg.str());
    }
}


void FenetreJeu::init()
{
    m_goban->init();
}


void FenetreJeu::abandon()
{

}

void FenetreJeu::passer()
{

}
