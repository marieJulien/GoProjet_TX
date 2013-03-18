#include "FenetreJeu.h"
#include "DebutJeu.h"
#include "../GobanFiles/GobanIA.h"
#include "../IA/IA.h"
#include "../Tools/CoupException.h"
#include "ResultatPartie.hpp"

FenetreJeu::FenetreJeu() : FenetrePrincipale()
{
//    std::cout << "Création fenêtre de jeu\n";
    QDesktopWidget desk;
    qreal m_height = desk.height();
    ECART = ceil((m_height-150)/(10));
    m_goban.reset(new GobanIA(ECART,9));
    boost::shared_ptr<GobanIA> gobanIA = boost::dynamic_pointer_cast<GobanIA>(m_goban);
    gobanIA->init();
    if (gobanIA->getPartieIA()->getCouleurIA()=="noir")
    {
        infosNoir->setNom(QString("IA"));
        infosBlanc->setNom(QString("Utilisateur"));
    }
    else
    {
        infosBlanc->setNom(QString("IA"));
        infosNoir->setNom(QString("Utilisateur"));
    }
    infosNoir->setCapt(QString("0"));
    infosBlanc->setCapt(QString("0"));

    vue = new QGraphicsView(m_goban.get());
    vue->setFixedSize(ECART*(m_goban->SIZE()+1),ECART*(m_goban->SIZE()+1));
    layoutV->addWidget(vue);


    /********* Définition d'une grille de boutons correspondant aux intersections *********/
    grilleBoutonsGoban =new QGridLayout();
    for (unsigned int i = 0; i<m_goban->SIZE(); i++)
    {
        for (unsigned int j = 0; j<m_goban->SIZE(); j++)
        {
            BoutonGoban* bouton =new BoutonGoban(i,j,m_goban);
            grilleBoutonsGoban->addWidget(bouton,j+1,i+1);
            connect(bouton,SIGNAL(clicked()),bouton,SLOT(envoyerSignalClicked()));
            connect(bouton,SIGNAL(clickedBouton(int,int)),this,SLOT(bouton_goban(int,int)));
        }
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


    widgetsCote->addSpacing(400);
}


void FenetreJeu::bouton_goban(int a, int o)
{
    try
    {
        std::cout << "Clicked : " << a << "-" << o << std::endl;
        boost::shared_ptr<GobanIA> gobanPtr = boost::dynamic_pointer_cast<GobanIA>(m_goban);

        if (!gobanPtr->getPartieIA()->partieFinie(gobanPtr))
        {
            if (gobanPtr->getPartieIA()->couleurAJouer()!=gobanPtr->getPartieIA()->getCouleurIA())
            {
                //si c'est bien à l'utilisateur de jouer
                passButton->setEnabled(false);
                resignButton->setEnabled(false);

                if (gobanPtr->coupPossible(a,o))
                {
                    std::cout << "\ncoup utilisateur possible\n";
                    boost::shared_ptr<Joueur> joueurUser = gobanPtr->getPartieIA()->getJoueur(gobanPtr->getPartieIA()->couleurAJouer());
                    Coup c(a,o,joueurUser);
                    c.setNum(gobanPtr->getPartieIA()->getListeCoups().size());
                    gobanPtr->getPartieIA()->ajouterCoup(c);
                    boost::shared_ptr<Pierre> pierrePtr(new Pierre(c,gobanPtr->ECART()));
                    int nbPierresCapturees = gobanPtr->ajouterPierre(pierrePtr).size();
                    joueurUser->addCapt(nbPierresCapturees);
                    std::ostringstream nbCapt;
                    nbCapt << joueurUser->getCapt();
                    getInfos(joueurUser->couleur())->setCapt(QString::fromStdString(nbCapt.str()));


                    std::cout << "coup de l'utilisateur : ok, maintenant choix de l'ia\n";

                    //ensuite, l'IA doit choisir un coup
                    tourIA();
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
    try
    {
        std::cout << "Clicked : PASS" << std::endl;
        boost::shared_ptr<GobanIA> gobanPtr = boost::dynamic_pointer_cast<GobanIA>(m_goban);

        if (!gobanPtr->getPartieIA()->partieFinie(gobanPtr))
        {
            if (gobanPtr->getPartieIA()->couleurAJouer()!=gobanPtr->getPartieIA()->getCouleurIA())
            {
                //si c'est bien à l'utilisateur de jouer
                passButton->setEnabled(false);
                resignButton->setEnabled(false);

                Coup dernierCoup = gobanPtr->getPartieIA()->getListeCoups().back();
                if (dernierCoup.getAbs()==-1)
                {
                    //l'IA a passé au coup d'avant : deux passes de suite = fin de la partie, il faut compter les points
                    //pas encore géré
                    //compter les scores, afficher une fenêtre d'info
                    std::cout << "Coup précédent = passer\n";
                    try
                    {
                        ResultatPartie* res = new ResultatPartie();
                        res->init(sharedFromThis_Jeu());
                        std::cout << "Création de la fenêtre de résultat : ok\n";
                        res->show();
                    }
                    catch(std::exception const& e)
                    {
                        std::cout << e.what();
                        throw coup_exception(e.what());
                    }
                }
                else
                {
                    boost::shared_ptr<Joueur> joueurUser = gobanPtr->getPartieIA()->getJoueur(gobanPtr->getPartieIA()->couleurAJouer());
                    Coup c(-1,-1,joueurUser);
                    c.setNum(gobanPtr->getPartieIA()->getListeCoups().size());
                    gobanPtr->getPartieIA()->ajouterCoup(c);

                    //au tour de l'IA
                    tourIA();
                }
            }
        }
        else
        {
            std::cout << gobanPtr->getPartieIA()->resultat(gobanPtr);
        }
    }
    catch(std::exception const& e)
    {
        throw coup_exception(e.what());
    }
}


void FenetreJeu::tourIA()
{
    boost::shared_ptr<GobanIA> gobanPtr = boost::dynamic_pointer_cast<GobanIA>(m_goban);

    //l'IA doit choisir un coup
    std::pair<int,int> coupIA = gobanPtr->getPartieIA()->getIA()->choixCoup();
    std::cout << "Choix de l'ia : " << coupIA.first << " - " << coupIA.second << std::endl;
    if ((coupIA.first==-1) && (coupIA.second==-1))
    {
        //L'IA a considéré que la partie était terminée = passer
        Coup dernierCoup = gobanPtr->getPartieIA()->getListeCoups().back();
        if (dernierCoup.getAbs()==-1)
        {
            //deux pass de suite -> fin de la partie
            //std::cout << gobanPtr->getPartieIA()->resultat(gobanPtr);
            //throw coup_exception("Deux passes de suite, fin de la partie !"+gobanPtr->getPartieIA()->resultat(gobanPtr));
            std::cout << "AAAAAAAAAAAAAAAAA\n";
            try
            {
                ResultatPartie* res = new ResultatPartie();
                res->init(sharedFromThis_Jeu());
                std::cout << "EEEEEEEEEEEEE";
                res->show();
            }
            catch(std::exception const& e)
            {
                std::cout << e.what();
                throw coup_exception(e.what());
            }
        }
        else
        {
            Coup c2(coupIA.first,coupIA.second,gobanPtr->getPartieIA()->getIA());
            c2.setNum(gobanPtr->getPartieIA()->getListeCoups().size());
            gobanPtr->getPartieIA()->ajouterCoup(c2);
        }

    }
    else
    {
        Coup c2(coupIA.first,coupIA.second,gobanPtr->getPartieIA()->getIA());
        c2.setNum(gobanPtr->getPartieIA()->getListeCoups().size());
        gobanPtr->getPartieIA()->ajouterCoup(c2);
        boost::shared_ptr<Pierre> pierre2 (new Pierre(c2,gobanPtr->ECART()));
        int nbPierresCapturees = gobanPtr->ajouterPierre(pierre2).size();
        gobanPtr->getPartieIA()->getIA()->addCapt(nbPierresCapturees);
    }

    passButton->setEnabled(true);
    resignButton->setEnabled(true);
}
