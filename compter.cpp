#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QGridLayout>
#include <QTableView>
#include <QSqlQuery>
#include <QSqlError>
#include <QHeaderView>
#include <QToolTip>
#include <QStackedWidget>
#include <QMenu>

#include "compter.h"
#include "db_tools.h"

void BCount::CreerCritereJours(void)
{
    QString st_tmp = "";

    QSqlQuery query(dbToUse) ;
    QString msg = "";
    QString st_table = "J";
    bool status = false;

#if 0
    if(db_data == TB2_BASE){
        st_table =  "jour_tirage";
    }
    else
    {
        st_table = "J";
    }
#endif
    msg = "select distinct substr(tb1."+st_table+",1,3) as J from ("+
            db_data+") as tb1 order by J asc;";

    status = query.exec(msg);

    if(status)
    {
        status = query.first();
        if (query.isValid())
        {
            do
            {
                //count(CASE WHEN  J like 'lundi%' then 1 end) as LUN,
                st_tmp = st_tmp + "count(CASE WHEN  J like '"+
                        query.value(0).toString()+"%' then 1 end) as "+
                        query.value(0).toString()+",";
            }while(status = query.next());

            //supprimer derniere ','
            st_tmp.remove(st_tmp.length()-1,1);
            st_tmp = st_tmp + " ";
        }
    }

#ifndef QT_NO_DEBUG
    qDebug() << "CreerCritereJours ->"<< query.lastError();
    qDebug() << "SQL 1:\n"<<msg<<"\n-------";
    qDebug() << "SQL 2:\n"<<st_tmp<<"\n-------";
#endif

    query.finish();
    db_jours = st_tmp;
}

void BCount::RecupererConfiguration(void)
{
    QSqlQuery query(dbToUse) ;
    QString msg = "";
    bool isOk = false;

    msg = "select count(id) as tot from (" + QString::fromLocal8Bit(C_TBL_1) + ");";
    isOk = query.exec(msg);

    if(isOk)
    {
        isOk = query.first();
        if (query.isValid())
        {
            nbZone = query.value(0).toInt();
            // J'assume que si la requete retourne qq chose
            // alors il y a au moins une zone existante
            lesSelections = new QModelIndexList [nbZone];
            sqlSelection = new QString [nbZone];
            memo = new int [nbZone];
            memset(memo,-1, sizeof(int)*nbZone);

            names  = new cZonesNames [nbZone];
            limites = new cZonesLimits [nbZone];
            sqmZones = new QSqlQueryModel [nbZone];


            // remplir les infos
            msg = "select tb1.id, tb1.std, tb1.abv, tb1.len, tb1.min, tb1.max, tb1.win from " +
                    QString::fromLocal8Bit(C_TBL_1) + " as tb1;";
            isOk = query.exec(msg);

            if(isOk)
            {
                isOk = query.first();
                if (query.isValid())
                {
                    for(int i = 0; (i< nbZone) && isOk; i++)
                    {
                        names[i].selection = "";
                        names[i].complet = query.value(1).toString();
                        names[i].court = query.value(2).toString();
                        limites[i].len = query.value(3).toInt();
                        limites[i].min = query.value(4).toInt();
                        limites[i].max = query.value(5).toInt();
                        limites[i].neg = query.value(6).toInt();

                        if(i<nbZone-1)
                            isOk = query.next();
                    }
                }
            }
        }
    }

    if(!isOk)
    {
        QString ErrLoc = "RecupererConfiguration:";
        DB_Tools::DisplayError(ErrLoc,&query,msg);
    }

    query.finish();

}

BCount::BCount(QString *in,QSqlDatabase useDb):BCount(in,useDb,NULL)
{
}

BCount::BCount(QString *in, QSqlDatabase fromDb, QWidget *unParent=0)
    :QWidget(unParent), db_data(*in),dbToUse(fromDb)
{
    nbZone = 0;
    db_jours = "";
    lesSelections = NULL;
    sqlSelection = NULL;
    memo = NULL;
    names = NULL;
    limites = NULL;
    sqmZones = NULL;

    RecupererConfiguration();
    CreerCritereJours();
}

void BCount::slot_AideToolTip(const QModelIndex & index)
{
    QString msg="";
    const QAbstractItemModel * pModel = index.model();
    int col = index.column();

    QVariant vCol = pModel->headerData(col,Qt::Horizontal);
    QString headName = vCol.toString();

    if (col >=1)
    {
        QString s_nb = index.model()->index(index.row(),0).data().toString();
        QString s_va = index.model()->index(index.row(),col).data().toString();
        QString s_hd = headName;
        msg = msg + QString("%1 tirage(s) \nayant %2 boule(s)%3").arg(s_va).arg(s_nb).arg(s_hd);
    }
    if(msg.length())
        QToolTip::showText (QCursor::pos(), msg);
}

void BCount::slot_ClicDeSelectionTableau(const QModelIndex &index)
{
    // L'onglet implique le tableau...
    int tab_index = 0;
    QTableView *view = qobject_cast<QTableView *>(sender());
    QStackedWidget *curOnglet = qobject_cast<QStackedWidget *>(view->parent()->parent());
    QItemSelectionModel *selectionModel = view->selectionModel();
    tab_index = curOnglet->currentIndex();

    lesSelections[tab_index]= selectionModel->selectedIndexes();
    LabelFromSelection(selectionModel,tab_index);
}


QString BCount::CriteresCreer(QString critere , QString operateur, int zone)
{
    QString ret_msg = "";

    // Operateur : or | and
    // critere : = | <>
    int totElements = limites[zone].len;
    for(int i = 0; i<totElements;i++)
    {
        QString zName = names[zone].court;
        ret_msg = ret_msg +"tb2.B "+ critere +" tb1."
                + zName+QString::number(i+1)
                + " " + operateur+ " ";
    }
    int len_flag = operateur.length();
    ret_msg.remove(ret_msg.length()-len_flag-1, len_flag+1);

#ifndef QT_NO_DEBUG
    qDebug() << ret_msg;
#endif

    return ret_msg;
}
QString BCount::CriteresAppliquer(QString st_tirages, QString st_cri, int zn)
{
}

void BCount::LabelFromSelection(const QItemSelectionModel *selectionModel, int zn)
{
    QModelIndexList indexes = selectionModel->selectedIndexes();
    QString str_titre = names[zn].court + "[";

    int nb_items = indexes.size();
    if(nb_items)
    {
        QModelIndex un_index;
        int curCol = 0;
        int occure = 0;

        /// Parcourir les selections
        foreach(un_index, indexes)
        {
            const QAbstractItemModel * pModel = un_index.model();
            curCol = pModel->index(un_index.row(), un_index.column()).column();
            occure = pModel->index(un_index.row(), 0).data().toInt();

            // si on n'est pas sur la premiere colonne
            if(curCol)
            {
                QVariant vCol;
                QString headName;

                vCol = pModel->headerData(curCol,Qt::Horizontal);
                headName = vCol.toString();
                str_titre = str_titre + "("+headName+"," + QString::number(occure) + "),";
            }
        }

        // supression derniere ','
        str_titre.remove(str_titre.length()-1,1);

        // on marque la fin
        str_titre = str_titre +"]";
    }
    else
    {
        str_titre = "";
    }

    // On sauvegarde la selection en cours
    names[zn].selection = str_titre;

    // on construit le nouveau titre
    str_titre = "";
    int isVide = 0;
    for(int i=0; i< nbZone; i++)
    {
        if(names[i].selection != ""){
            str_titre = str_titre + names[i].selection+",";
        }
        else
        {
            isVide++;
        }
    }
    // retirer la derniere ','
    str_titre.remove(str_titre.length()-1,1);

    // Tout est deselectionné ?
    if(isVide == nbZone)
    {
        str_titre = "Aucun";
    }

    // informer disponibilité
    emit sig_TitleReady(str_titre);
}


/// Cette fonction cherche dans la table designée si une valeur est presente
/// auquel cas le champs situe a idColValue est aussi retourné
/// item : valeur a rechercher
/// table : nom de la table dans laquelle il faut chercher
/// idColValue colonne de la table ou se trouve la valeur
/// *lev : valeur de priorité trouvé
bool BCount::VerifierValeur(int item,QString table,int idColValue,int *lev)
{
    bool ret = false;
    QSqlQuery query(dbToUse) ;
    QString msg = "";

    /// La colonne val sert de foreign key
    msg = "select * from " + table + " " +
            "where (val = "+QString::number(item)+");";
    ret =  query.exec(msg);

    if(!ret)
    {
#ifndef QT_NO_DEBUG
        qDebug() << "select: " <<table<<"->"<< query.lastError();
        qDebug() << "Bad code:\n"<<msg<<"\n-------";
#endif
    }
    else
    {
#ifndef QT_NO_DEBUG
        qDebug() << "Fn VerifierValeur:\n"<<msg<<"\n-------";
#endif

        // A t on un resultat
        ret = query.first();
        if(query.isValid())
        {
            int val = query.value(idColValue).toInt();
            *lev = val;
        }
    }
    return ret;
}

void BCount::slot_ccmr_tbForBaseEcart(QPoint pos)
{
    /// http://www.qtcentre.org/threads/7388-Checkboxes-in-menu-items
    /// https://stackoverflow.com/questions/2050462/prevent-a-qmenu-from-closing-when-one-of-its-qaction-is-triggered

    QTableView *view = qobject_cast<QTableView *>(sender());
    QModelIndex index = view->indexAt(pos);
    int col = view->columnAt(pos.x());

    if(col == 0)
    {
        QString tbl = view->objectName();

        int val = 0;
        if(index.model()->index(index.row(),col).data().canConvert(QMetaType::Int))
        {
            val =  index.model()->index(index.row(),col).data().toInt();
        }

        QMenu *MonMenu = new QMenu(this);
        QMenu *subMenu= ContruireMenu(tbl,val);
        MonMenu->addMenu(subMenu);
        CompleteMenu(MonMenu, tbl, val);


        MonMenu->exec(view->viewport()->mapToGlobal(pos));
    }
}

QMenu *BCount::ContruireMenu(QString tbl, int val)
{
    QString msg2 = "Priorite";
    QMenu *menu =new QMenu(msg2, this);
    QActionGroup *grpPri = new  QActionGroup(menu);

    int col = 2; /// dans la table colonne p
    int niveau = 0;
    bool existe = false;
    existe = VerifierValeur(val, tbl,col,&niveau);



    /// Total de priorite a afficher
    for(int i =1; i<=5;i++)
    {
        QString name = QString::number(i);
        QAction *radio = new QAction(name,grpPri);
        name = QString::number(existe)+
                ":"+QString::number(niveau)+
                ":"+name+":"+QString::number(val)+
                ":"+tbl;
        radio->setObjectName(name);
        radio->setCheckable(true);
        menu->addAction(radio);
    }

    QAction *uneAction;
    if(niveau)
    {
        uneAction = qobject_cast<QAction *>(grpPri->children().at(niveau-1));
        uneAction->setChecked(true);
    }
    connect(grpPri,SIGNAL(triggered(QAction*)),this,SLOT(slot_ChoosePriority(QAction*)));
    return menu;
}


/// Selectionner une priorite de choix pour une boule
/// Cela conduira a la mettre dans un ensemble pour generer les jeux posibles
void BCount::slot_ChoosePriority(QAction *cmd)
{
    QSqlQuery query(dbToUse);
    QString msg = "";

    QString st_from = cmd->objectName();
    QStringList def = st_from.split(":");
    /// Verifier coherence des donnees
    /// pos 0: ligne trouvee dans table
    /// pos 1: ancie priorite
    /// pos 2: nvlle priorite
    /// pos 3: element selectionne
    /// pos 4:nom de table
    if(def.size()!=5)
        return;

    int trv = def[0].toInt();
    int v_1 = def[1].toInt();
    int v_2 = def[2].toInt();
    int elm = def[3].toInt();
    QString tbl = def[4];

    // faut il inserer une nouvelle ligne
    /// TB_SE
    if(trv ==0)
    {
        msg = "insert into " + tbl + " (id, val, p, f) values(NULL,"
                +def[3]+","+ def[2]+",0);";

    }
    // Verifier si if faut supprimer la priorite
    if(v_1 == v_2)
    {
        msg = "update " + tbl + " set p=0 "+
                "where (val="+def[3]+");";
        trv = 0;
    }

    // faut il une mise a jour ?
    if((v_1 != v_2)&& (trv!=0))
    {
        msg = "update " + tbl + " set p="+def[2]+" "+
                "where (val="+def[3]+");";

    }

    bool rep = query.exec(msg);

    if(!rep)
    {
        trv = false;
#ifndef QT_NO_DEBUG
        qDebug() << "select: " <<def[3]<<"->"<< query.lastError();
        qDebug() << "Bad code:\n"<<msg<<"\n-------";
#endif
    }
    else
    {
        trv = true;
#ifndef QT_NO_DEBUG
        qDebug() << "Fn :\n"<<msg<<"\n-------";
#endif

    }

    /// montrer que l'on a compris
    /// la demande utilisateur
    cmd->setChecked(true);
}

void BCount::CompleteMenu(QMenu *LeMenu,QString tbl, int clef)
{
    int col = 3; /// dans la table colonne "f"
    int niveau = 0;
    bool existe = false;
    existe = VerifierValeur(clef, tbl,col,&niveau);

    QAction *filtrer = LeMenu->addAction("Filtrer");
    filtrer->setCheckable(true);

    int i = 0;
    QString name = QString::number(i);
    name = QString::number(existe)+
            ":"+QString::number(niveau)+
            ":"+name+":"+QString::number(clef)+
            ":"+tbl;

    filtrer->setObjectName(name);
    filtrer->setChecked(niveau);
    connect(filtrer,SIGNAL(triggered(bool)),
            this,SLOT(slot_wdaFilter(bool)));
}

/// https://openclassrooms.com/forum/sujet/qt-inclure-check-box-dans-un-menu-deroulant-67907
void BCount::slot_wdaFilter(bool val)
{
    QAction *chkFrom = qobject_cast<QAction *>(sender());

#ifndef QT_NO_DEBUG
    qDebug() << "Boule :("<< chkFrom->objectName()<<") check:"<< val;
#endif
    QSqlQuery query(dbToUse);
    QString msg = "";

    QString st_from = chkFrom->objectName();
    QStringList def = st_from.split(":");
    /// Verifier coherence des donnees
    /// pos 0: ligne trouvee dans table
    /// pos 1: ancie priorite
    /// pos 2: nvlle priorite
    /// pos 3: element selectionne
    /// pos 4:nom de table
    if(def.size()!=5)
        return;

    int trv = def[0].toInt();
    int v_1 = def[1].toInt();
    int v_2 = def[2].toInt();
    int elm = def[3].toInt();
    QString tbl = def[4];

    // faut il inserer une nouvelle ligne CREER UNE VARIABLE POUR LES COLONNES
    /// TB_SE
    if(trv ==0)
    {
        msg = "insert into " + tbl + " (id, val, p, f) values(NULL,"
                +def[3]+",0,"+QString::number(val)+");";

    }
    else
    {
        msg = "update " + tbl + " set f="+QString::number(val)+" "+
                "where (val="+def[3]+");";
    }

    bool rep = query.exec(msg);

    if(!rep)
    {
        trv = false;
#ifndef QT_NO_DEBUG
        qDebug() << "select: " <<def[3]<<"->"<< query.lastError();
        qDebug() << "Bad code:\n"<<msg<<"\n-------";
#endif
    }
    else
    {
        trv = true;
#ifndef QT_NO_DEBUG
        qDebug() << "Fn :\n"<<msg<<"\n-------";
#endif

    }

    /// Recharger les reponses dans le tableau
    int zn = tbl.split("z").at(1).toInt() - 1;
    QString Montest = sqmZones[zn].query().executedQuery();
    qDebug() << Montest;
    sqmZones[zn].setQuery(Montest,dbToUse);

    delete chkFrom;
}

