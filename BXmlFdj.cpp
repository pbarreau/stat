#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QMessageBox>
#include <QDomComment>
#include "BXmlFdj.h"

static QString key_1 = "Type";

BXmlFdj::BXmlFdj(etFdj rungame)
{

    QString targetFile = "ConfStatFdj.xml" ;
    QFile xmlFile(targetFile);


    QDomDocument document;

    QDomElement root = document.createElement("Historiques Fdj");
    document.appendChild(root);

    QDomElement lstGames = document.createElement("Types de jeux"); // QDomElement
    root.appendChild(lstGames);

    QString msg_comment = " Nombre de types possibles : "+ QString::number(eFdjEol-1).rightJustified(2,'0') + " ";
    QDomComment un_commentaire = document.createComment(msg_comment);
    lstGames.appendChild(un_commentaire);

    for(int i = 1; i<eFdjEol;i++){
        msg_comment = " Type:"+ QString::number(i).rightJustified(2,'0') + " ";
        un_commentaire = document.createComment(msg_comment);
        lstGames.appendChild(un_commentaire);

        QDomElement game = document.createElement(key_1);
        game.setAttribute("Id",i);
        game.setAttribute("Nom",TXT_Game[i]);
        game.setNodeValue(TXT_Game[i]);
        lstGames.appendChild(game);

        mkDomGame((etFdj)i, &document,lstGames);
    }




    if(!xmlFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
#ifndef QT_NO_DEBUG
        qDebug() << "Open the file for writing failed";
#endif
    }
    else
    {
        QTextStream stream(&xmlFile);
        stream << document.toString();
        xmlFile.close();
#ifndef QT_NO_DEBUG
        qDebug() << "Writing is done";
#endif
    }
}

void BXmlFdj::mkDomGame(etFdj fdjType, QDomDocument *doc, QDomElement target)
{
    QDomNodeList att = target.elementsByTagName(key_1);
    int nbItems = att.size();

    const stSrcHistoJeux *ptr_histo =nullptr;
    //const stParam_1 *ptr_prm1 = nullptr;
    //const stParam_2 *ptr_prm2 = nullptr;
    const QString   *ptr_names = nullptr;
    int i_deb = -1;
    int i_end = -1;
    int i_tot = -1;
    //int i_pr1 = -1;
    //int i_pr2 = -1;


    switch (fdjType) {
    case eFdjLoto:
        i_deb = eCnameLoto;
        i_end = eFdjEndCnames_1 - eCnameLoto;
        ptr_names = TXT_FdjLst_1;
        //ptr_histo = &HistoLoto[0];
        i_tot = sizeof(HistoLoto)/sizeof(stSrcHistoJeux);
#if 0
        ptr_prm1 = &loto_prm1_zn[0];
        i_pr1 = sizeof(loto_prm1_zn)/sizeof(stParam_1);
        ptr_prm2 = &loto_prm2_zn[0];
        i_pr2 = sizeof(loto_prm2_zn)/sizeof(stParam_1);
#endif
        break;
    case eFdjEuro:
        i_deb = eCnameEuroMillionsMyMillion;
        i_end = eFdjEndCnames_2 - eCnameEuroMillionsMyMillion;
        ptr_names = TXT_FdjLst_2;
        //ptr_histo = &HistoEuro[0];
        i_tot = sizeof(HistoEuro)/sizeof(stSrcHistoJeux);
#if 0
        ptr_prm1 = &loto_prm1_zn[0];
        i_pr1 = sizeof(euro_prm1_zn)/sizeof(stParam_1);
        ptr_prm2 = &euro_prm2_zn[0];
        i_pr2 = sizeof(loto_prm2_zn)/sizeof(stParam_1);
#endif
        break;
    default:
        break;
    }

    // Trouver le noeud
    QDomNode cible = QDomNode();
    for(int i=0; i< nbItems;i++){
        QString valeur = att.at(i).nodeValue();
        if(valeur.compare(TXT_Game[fdjType])==0){
            cible = att.at(i);
            break;
        }
    }

    /// Trouve ?
    if(!cible.isNull()){
        QString msg_comment = "Nombre de fichiers total : "+ QString::number(i_tot).rightJustified(2,'0');
        QDomComment un_commentaire = doc->createComment(msg_comment);
        cible.appendChild(un_commentaire);


        for(int j=0; j<i_end;j++){
            eFCname item = (eFCname) (j+i_deb);
            QString msg_comment = "Appelation : "+QString::number(j+1).rightJustified(2,'0');
            QDomComment un_commentaire = doc->createComment(msg_comment);
            cible.appendChild(un_commentaire);

            QDomElement CName = doc->createElement("Appelation"); // nom commercial
            CName.setAttribute("Id",j+1);
            CName.setAttribute("Nom",ptr_names[j]);
            mkDomHisto(fdjType, item,doc,CName);
            cible.appendChild(CName);
        }
    }
}

void BXmlFdj::mkDomHisto(etFdj fdjType, eFCname game,
                         QDomDocument *doc, QDomElement target)
{
    const stSrcHistoJeux *ptr_histo =nullptr;
    int i_tot = -1;

    switch (fdjType) {
    case eFdjLoto:
        ptr_histo = &HistoLoto[0];
        i_tot = sizeof(HistoLoto)/sizeof(stSrcHistoJeux);
        break;
    case eFdjEuro:
        ptr_histo = &HistoEuro[0];
        i_tot = sizeof(HistoEuro)/sizeof(stSrcHistoJeux);
        break;
    default:
        break;
    }

    int nbItems = i_tot;
    int i = 0;
    int p = 0;

    for(i=0;i<nbItems;i++){
        if(ptr_histo[i].type != game){
            p++;
            continue;
        }

        if(ptr_histo[i].type == game){
            int j=i;
            do{
                QDomElement src = doc->createElement("Fichier");
                src.setAttribute("Id",p+1);
                src.setAttribute("Nom",ptr_histo[j].file);
                src.setAttribute("Txt",ptr_histo[j].memo);
                src.setAttribute("Url",ptr_histo[j].http);
                target.appendChild(src);
                mkDomFichier(fdjType, j, doc, src);
                p++;
                j++;
            }while((ptr_histo[j].type == game) && (j<nbItems));
            i=j;
        }
    }

}

void BXmlFdj::mkDomFichier(etFdj fdjType, int file, QDomDocument *doc, QDomElement target)
{
    const stParam_1 *ptr_prm1 = nullptr;
    const stParam_2 *ptr_prm2 = nullptr;
    int i_pr1 = -1;
    int i_pr2 = -1;


    switch (fdjType) {
    case eFdjLoto:
        ptr_prm1 = &loto_prm1_zn[0];
        i_pr1 = sizeof(loto_prm1_zn)/sizeof(stParam_1);
        ptr_prm2 = &loto_prm2_zn[0];
        i_pr2 = sizeof(loto_prm2_zn)/sizeof(stParam_2);
        break;
    case eFdjEuro:
        ptr_prm1 = &euro_prm1_zn[0];
        i_pr1 = sizeof(euro_prm1_zn)/sizeof(stParam_1);
        ptr_prm2 = &euro_prm2_zn[0];
        i_pr2 = sizeof(euro_prm2_zn)/sizeof(stParam_2);
        break;
    default:
        break;
    }

    /// Verifiation configuration sans fautes
    if( i_pr1 != i_pr2){
        QMessageBox myMsg;
        myMsg.setText("Erreur Configuration !!");
        myMsg.setDetailedText("Sur definitions des zones");
        myMsg.setSizeGripEnabled(true);
        myMsg.setIcon(QMessageBox::Critical);
        myMsg.exec();
        return;
    }

    QDomElement lgn = doc->createElement("Structure");
    lgn.setAttribute("Jour",2);
    lgn.setAttribute("Date",3);
    lgn.setAttribute("NbZone", i_pr1);

    for(int id_zn=0; id_zn < i_pr1; id_zn++){
        QDomElement zn = doc->createElement("Zone");
        zn.setAttribute("Id",id_zn+1);

        QDomElement data = doc->createElement("Description");
        data.setAttribute("Deb", ptr_prm1[id_zn].pos);
        data.setAttribute("Len", ptr_prm1[id_zn].len);
        data.setAttribute("ValMin", ptr_prm1[id_zn].min);
        data.setAttribute("ValMax", ptr_prm1[id_zn].max);
        data.setAttribute("NbToWin", ptr_prm1[id_zn].win);

        data.setAttribute("StdName", ptr_prm2[id_zn].std);
        data.setAttribute("AbvName", ptr_prm2[id_zn].abv);

        zn.appendChild(data);
        lgn.appendChild(zn);

    }
    target.appendChild(lgn);
}
