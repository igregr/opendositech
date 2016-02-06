#include "database.h"

DB* DB::m_instance = new DB();


DB::DB()
{
}

bool DB::init()
{
    if (createConnection()) {
        if (!attachDatabases()) {
            qFatal("Chyba u pripojeni databazi");
            return false;
        }
    } else {
        qFatal("Chyba pri otevreni databaze ");
        return false;
    }    
    return true;
}


DB::~DB()
{
}

void DB::closeDatabase()
{
    db.close();
    db = QSqlDatabase();
}

bool DB::initDatabase()
{
    if (ctSeznamDavek() && ctDoklady() && ctVykony() && ctKartoteka() && ctIDs()) {
        return true;    
    }
    return false;
}

/*vytvori tabulku seznam_davek*/
bool DB::ctSeznamDavek()
{
    QString command = "CREATE TABLE seznam_davek ("
                                                    "id_davky INTEGER PRIMARY KEY,"
                                                    "charakter CHAR(1),"
                                                    "typ CHAR(2),"
                                                    "icz CHAR(8),"
                                                    "pob CHAR(4),"
                                                    "rok INTEGER(4),"
                                                    "mesic INTEGER(2),"
                                                    "cislo_davky CHAR(6),"
                                                    "pojistovna CHAR(3),"
                                                    "pocet_dokladu INTEGER,"
                                                    "pocet_bodu INTEGER,"
                                                    "vytvoreni_davky DATE"
                                                ")";
        if (!executeCommand(command)) {
                qFatal("Nepodarilo se vytvorit tabulku doklady");
                return false;
        }
        command =   "CREATE TRIGGER delete_davka BEFORE DELETE ON seznam_davek"
                    " BEGIN"
                    " DELETE FROM doklady WHERE id_davky = old.id_davky;"
                    " END";
        if (!executeCommand(command)) {
                qFatal("Nepodarilo se vytvorit trigger delete_davka");
                return false;
        }
        return true;
}

/*vytvori tabulku doklady*/
bool DB::ctDoklady()
{
    QString command = "CREATE TABLE doklady ("
                                                "id_dokladu INTEGER PRIMARY KEY,"
                                                "id_davky INTEGER,"
                                                "cislo_dokladu INTEGER(7),"
                                                "icp CHAR(8),"
                                                "odbornost CHAR(3),"
                                                "var_symbol CHAR(6),"
                                                "rodne_cislo CHAR(10),"
                                                "zakl_diagnoza CHAR(5),"
                                                "body_celkem INTEGER,"
                                                "vykony_celkem INTEGER"
                                           ")";
    if (!executeCommand(command)) {
        qFatal("Nepodarilo se vytvorit tabulku doklady");
        return false;
    }
    command = "CREATE TRIGGER insert_into_doklady AFTER INSERT ON doklady"
              " BEGIN"
              "  UPDATE seznam_davek SET pocet_dokladu=pocet_dokladu+1 WHERE id_davky=new.id_davky;"
              " END";
    if (!executeCommand(command)) {
        qFatal("Nepodarilo se vytvorit trigger insert_into_doklady");
        return false;
    }
    command = "CREATE TRIGGER delete_doklad DELETE ON doklady"
              " BEGIN"
              "  UPDATE seznam_davek SET pocet_dokladu=pocet_dokladu-1 WHERE id_davky=old.id_davky;"
              "  DELETE FROM vykony WHERE id_dokladu= old.id_dokladu;"
              " END";
    if (!executeCommand(command)) {
        qFatal("Nepodarilo se vytvorit trigger delete_doklad");
        return false;
    }
    command = "CREATE TRIGGER update_doklady AFTER UPDATE OF body_celkem ON doklady"
              " BEGIN"
              "  UPDATE seznam_davek SET pocet_bodu=pocet_bodu + (new.body_celkem - old.body_celkem) WHERE id_davky=new.id_davky;"
              " END";
    if (!executeCommand(command)) {
        qFatal("Nepodarilo se vytvorit trigger update_doklady");
        return false;
    }
    command = "CREATE INDEX iid ON doklady(id_davky)";
    if (!executeCommand(command)) {
        qFatal("Nepodarilo se vytvorit index");
        return false;
    }
    command = "CREATE INDEX icd ON doklady(cislo_dokladu)";
    if (!executeCommand(command)) {
        qFatal("Nepodarilo se vytvorit index");
        return false;
    }
    command = "CREATE INDEX iidv ON doklady(id_dokladu)";
    return executeCommand(command) ? true : false;
}

/*vytvori tabulku vykony*/
bool DB::ctVykony()
{   
    QString command = "CREATE TABLE vykony ("
                                              "id_vykonu INTEGER PRIMARY KEY,"
                                              "id_dokladu INTEGER,"
                                              "datum DATE,"
                                              "kod CHAR(5),"
                                              "pocet INTEGER(1),"
                                              "odbornost CHAR(3),"
                                              "diagnoza CHAR(5),"
                                              "body INTEGER"
                                          ")";
    if (!executeCommand(command)) {
        qFatal("Nepodarilo se vytvorit tabulku vykony");
        return false;
    }
                        
    command =   "CREATE TRIGGER update_vykony AFTER UPDATE OF body ON vykony "
                " BEGIN "
                "  UPDATE doklady SET body_celkem=body_celkem + (new.body - old.body) WHERE id_dokladu=new.id_dokladu; "
                " END";
    if (!executeCommand(command)) {
        qFatal("Nepodarilo se vytvorit trigger update_vykony");
        return false;
    }
                        
    command =   "CREATE TRIGGER insert_into_vykony AFTER INSERT ON vykony "
                " BEGIN "
                "  UPDATE doklady SET body_celkem=body_celkem+new.body, vykony_celkem=vykony_celkem+1 WHERE id_dokladu=new.id_dokladu;"
                " END";
    if (!executeCommand(command)) {
        qFatal("Nepodarilo se vytvorit trigger insert_into_vykony");
        return false;
    }
                        
    command =   "CREATE TRIGGER delete_vykon DELETE ON vykony "
                " BEGIN "
                "  UPDATE doklady SET body_celkem=body_celkem-old.body, vykony_celkem=vykony_celkem-1 WHERE id_dokladu=old.id_dokladu;"
                " END";
    if (!executeCommand(command)) {
        qFatal("Nepodarilo se vytvorit trigger delete_vykon");
        return false;
    }
    command = "CREATE INDEX ivykon ON vykony(id_dokladu)";
    return executeCommand(command) ? true : false;
}

bool DB::ctKartoteka()
{
   QString command = "CREATE TABLE kartoteka ("
                                              "id_pacient INTEGER PRIMARY KEY,"
                                              "rodne_cislo text UNIQUE,"
                                              "jmeno TEXT,"
                                              "prijmeni TEXT,"
                                              "pojistovna TEXT,"
                                              "datum_vlozeni TEXT,"
                                              "datum_registrace TEXT"
                                          ")";
    if (!executeCommand(command)) {
        qFatal("Nepodarilo se vytvorit tabulku kartoteka");
        return false;
    } 
    return true;
}

bool DB::ctIDs()
{
   QString command = "CREATE TABLE id_davky ("
                                              "id INTEGER,"
                                              "rok INTEGER"
                                          ")";
    if (!executeCommand(command)) {
        qFatal("Nepodarilo se vytvorit tabulku id_davky");
        return false;
    } 

    command = "CREATE TABLE id_doklady ("
                                              "id INTEGER,"
                                              "rok INTEGER"
                                          ")";
    if (!executeCommand(command)) {
        qFatal("Nepodarilo se vytvorit tabulku id_doklady");
        return false;
    } 
    return true;
}



bool DB::checkQuery(const QSqlQuery &query)
{
    if (query.lastError().isValid()) {
        qCritical(query.lastQuery().toLatin1());
        qCritical(query.lastError().text().toLatin1());
        return false;
    }
    return true;
}

bool DB::executeCommand(const QString command, bool find)
{
    QSqlQuery query(command);
    if (checkQuery(query)) { // query ok
        qDebug() << "SQLCOMMAND " << command;
        if (find) {
            return query.next() ? true : false; 
        }
        return true;
    }

    return false;
}

bool DB::createConnection()
{
    bool database_exists = QFile::exists("odDatabase.sqlite");
    
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("odDatabase.sqlite");
    if (!db.open()) {
        qFatal(db.lastError().text().toLatin1());
        return false;
    }
    if (database_exists == false) { // databaze neexistovaa, vytvarim novou
        db.exec( "PRAGMA encoding = \"UTF-16\"" );
        if (!initDatabase()) {
            qFatal("Chyba pri inicializaci databaze");
            return false;
        }
    }
    return true;
}

bool DB::attachDatabases()
{
    QString command;
    if (QFile::exists("cis/poj.db")) {
        command = "ATTACH DATABASE 'cis/poj.db' as database_poj";
        if (!executeCommand(command)) {
            return false;
        }
    } else {
        qDebug() << "poj.db neexistuje";
        return false;
    }
    
    if(QFile::exists("cis/vykony.db")){
        command = "ATTACH DATABASE 'cis/vykony.db' as database_vykony";
        if (!executeCommand(command)) {
            return false;
        }
    } else {
        qDebug() << "vykony.db neexistuje"; 
        return false;
    }
    if (QFile::exists("cis/mkn10.db")) {
        command = "ATTACH DATABASE 'cis/mkn10.db' as database_mkn10";
        if (!executeCommand(command)) {
            return false;
        }
    } else {
        qDebug() << "mkn10.db neexistuje";
        return false;   
    }
    if (QFile::exists("cis/ciselpob.db")) {
        command = "ATTACH DATABASE 'cis/ciselpob.db' as database_pobocky";
        if (!executeCommand(command)) {
            return false;
        }
    } else {
        qDebug() << "mkn10.db neexistuje";  
        return false;
    }
    return true;
}

bool DB::isInDatabase(const QString command)
{
    if (executeCommand(command, true)) {
        qDebug() << "DATABASE: Polozka nalezena";
        return true;
    } else {
        qDebug() << "DATABASE: Polozka nenalezena";
        return false;
    }
}


bool DB::isInsuranceOffice(const QString pob)
{
    QString command = QString("SELECT 'database_pobocky' cislo_pracoviste from cis_pobocky where cislo_pracoviste='%1'").arg(pob);
    return isInDatabase(command) ? true : false;
}

bool DB::isInsuranceCompany(const QString company)
{
    QString command = QString("SELECT 'poj', code FROM inComp WHERE code='%1' LIMIT 1").arg(company);
    return isInDatabase(command) ? true : false;
}


qint64 DB::createNewBatchSignature(CrateBatch bi)
{
/* charakter davky 
 *   P - puvodni davka (doklady poprve predkladane)
 *   O - opravna davka (doklady opravene)
 *   Z - zaplacena davka - NEpouzivaji smluvni partneri, 
 */

    QString command = QString("INSERT INTO seznam_davek (charakter, typ, icz, pob, cislo_davky, pojistovna, pocet_dokladu, pocet_bodu) "
                              "VALUES('%1','%2','%3','%4','%5','%6','%7','%8')"
                ).arg(bi.charakter,bi.typ,bi.icz,bi.pob,bi.cislo_davky,bi.pojistovna,bi.pocet_dokladu,bi.pocet_bodu);

    return getLastId(command);

}

int DB::getBatchMonth(const QString id_davky)
{
    QString command = QString("SELECT cislo_dokladu FROM doklady WHERE id_davky='%1' LIMIT 1").arg(id_davky);
    QSqlQuery query(command);
    if (checkQuery(query)) { // query ok
        if (query.next()) {
            QString cd = query.value(0).toString();
            if (cd.size() == 6) {
                cd = cd.left(2);
                return cd.toInt();
                        } else if (cd.size() == 5){
                cd = cd.left(1);
                return cd.toInt();
                        }
        }
    }
    return -1;
}

QList<CrateBatch> DB::getBatchInfo(const QString batch_number, const QString in_comp_number)
{
//    QStringList batch_info;
    QList<CrateBatch> data;
    QString command = QString("SELECT id_davky, charakter, typ, icz, pob, rok, mesic, cislo_davky, "
                              "pojistovna, pocet_dokladu, pocet_bodu, vytvoreni_davky FROM seznam_davek "
                              "WHERE cislo_davky='%1' AND pojistovna='%2'").arg(batch_number).arg(in_comp_number);
    QSqlQuery query(command);
    if (checkQuery(query)) { //query ok
        while (query.next()) { //nactu davky
            CrateBatch batch_info;
            batch_info.id_davky         = query.value(BT_ID_DAVKY).toString();
            batch_info.charakter        = query.value(BT_CHARAKTER).toString();
            batch_info.typ              = query.value(BT_TYP).toString();
            batch_info.icz              = query.value(BT_ICZ).toString();
            batch_info.pob              = query.value(BT_POB).toString();
            batch_info.rok              = query.value(BT_ROK).toString();
            batch_info.mesic            = query.value(BT_MESIC).toString();
            batch_info.cislo_davky      = query.value(BT_CISLO_DAVKY).toString();
            batch_info.pojistovna       = query.value(BT_POJISTOVNA).toString();
            batch_info.pocet_dokladu    = query.value(BT_POCET_DOKLADU).toString();
            batch_info.pocet_bodu       = query.value(BT_POCET_BODU).toString();
            batch_info.vytvoreni_davky  = query.value(BT_VYTVORENI_DAVKY).toString();
            data.append(batch_info);
        }
    }
    return data;
}


QString DB::getBatchID()
{
    QString ret;
    QString command = QString("SELECT max(id) + 1 FROM id_davky WHERE rok = '%1'").arg(QDate::currentDate().toString("yyyy"));
    QSqlQuery query;
    query.exec(command);
    if (checkQuery(query)) { // query ok
        if (query.next()) {
            ret = query.value(0).toString();
            if (ret.isEmpty()) {
                command = QString("INSERT into id_davky (id,rok) VALUES (1, %2)").arg(QDate::currentDate().toString("yyyy"));
                query.exec(command);
                return QString("%1").arg(1);
            }
            command = QString("INSERT into id_davky (id,rok) VALUES ('%1', %2)").arg(ret.toInt()).arg(QDate::currentDate().toString("yyyy"));
            query.exec(command);
            qDebug() << "ID " << ret;
        } else {
            command = QString("INSERT into id_davky (id,rok) VALUES ('1', %1)").arg(QDate::currentDate().toString("yyyy"));
            query.exec(command);
            return QString("%1").arg(1);
        }
    }
    return ret;
}

QString DB::getRecordID()
{
    QString ret;
    QString command = QString("SELECT max(id) + 1 FROM id_doklady WHERE rok = '%1'").arg(QDate::currentDate().toString("yyyy"));
    QSqlQuery query;
    query.exec(command);
    if (checkQuery(query)) { // query ok
        if (query.next()) {
            ret = query.value(0).toString();
            if (ret.isEmpty()) {
                command = QString("INSERT into id_doklady (id,rok) VALUES (1, %2)").arg(QDate::currentDate().toString("yyyy"));
                query.exec(command);
                return QString("%1").arg(1);
            }
            command = QString("INSERT into id_doklady (id,rok) VALUES ('%1', %2)").arg(ret.toInt()).arg(QDate::currentDate().toString("yyyy"));
            query.exec(command);
            qDebug() << "ID " << ret;
        } else {
            command = QString("INSERT into id_doklady (id,rok) VALUES ('1', %1)").arg(QDate::currentDate().toString("yyyy"));
            query.exec(command);
            return QString("%1").arg(1);
        }
    }
    return ret;
}



QString DB::getMaxRecordNumber(const QString id_davky)
{
    QString ret;
    QString command = QString("SELECT max(cislo_dokladu) FROM doklady WHERE id_davky='%1'").arg(id_davky);
    QSqlQuery query(command);
    if (checkQuery(query)) { // query ok
        if (query.next()) {
            ret = query.value(0).toString();
        }
    }
    return ret;
}


CrateRecordInfo DB::getRecordInfo(const QString batch_id, const QString record_number)
{
//TODO mrknout na rowid, je zbytecne
QString command = QString("SELECT id_davky, cislo_dokladu, icp,odbornost,var_symbol,rodne_cislo,zakl_diagnoza,body_celkem,vykony_celkem,id_dokladu,rowid "
                              "FROM doklady WHERE id_davky=%1 AND cislo_dokladu=%2").arg(batch_id).arg(record_number);
    QSqlQuery query(command);
    CrateRecordInfo ri;
    if (checkQuery(command)) { // query ok
        if (query.next()) {
            ri.id_davky = query.value(0).toString();
            ri.cislo_dokladu = query.value(1).toString();
            ri.icp = query.value(2).toString();
            ri.odb = query.value(3).toString();
            ri.var_symbol = query.value(4).toString();
            ri.rodne_cislo = query.value(5).toString();
            ri.zakl_diag = query.value(6).toString();
            ri.body_celkem = query.value(7).toString();
            ri.vykony_celkem = query.value(8).toString();
            ri.id_vykonu = query.value(9).toString();
            ri.rowid = query.value(10).toLongLong();
        }
    }
    return ri;
}

QList<CrateRecordInfoData> DB::getRecordData(const QString record_id)
{

//TODO rowid
QList<CrateRecordInfoData> data;
    QString command = QString("SELECT datum,kod,pocet,odbornost,diagnoza,rowid FROM vykony WHERE id_dokladu= %1").arg(record_id);
    QSqlQuery query(command);
    if (checkQuery(command)) {
        while (query.next()) {
            CrateRecordInfoData d;
            d.datum = QDate::fromString(query.value(0).toString(), "dd.MM.yyyy");
            d.kod =  query.value(1).toString();
            d.pocet = query.value(2).toString();
            d.odb =  query.value(3).toString();
            d.diag = query.value(4).toString();
            d.rowid = query.value(5).toLongLong();

            data.append(d);
        }
    }
    return data;
}


DiagnoseStatus DB::isValidDiagnose(const QString diag)
{
    QString command = QString("SELECT ZNAK FROM mkn10 WHERE KOD='%1' LIMIT 1").arg(diag);
    QSqlQuery query(command);
    if (checkQuery(query)) {
        if (query.next()) {
            if (query.value(0).toString() == "*") {
                return DIAGNOSE_STAR;
            }
        return DIAGNOSE_OK;
        }
    }
    return DIAGNOSE_BAD;
}

qint64 DB::getLastId(QString command)
{
    QSqlQuery query(command);
    if (checkQuery(query)) {
        return query.lastInsertId().toLongLong();
    }
    return -1;

}


qint64 DB::saveRecordInfo(const CrateRecordInfo d)
{
    QString command = QString(
        "INSERT INTO doklady (id_davky,cislo_dokladu,icp,odbornost,var_symbol,rodne_cislo,zakl_diagnoza,body_celkem,vykony_celkem)"
        "VALUES(%1,%2,'%3','%4','%5','%6','%7',%8,%9)").arg(d.id_davky).arg(d.cislo_dokladu).arg(d.icp).arg(d.odb).arg(d.var_symbol).arg(d.rodne_cislo).arg(d.zakl_diag).arg(d.body_celkem).arg(d.vykony_celkem);
    return getLastId(command);
}
//TODO rowid
bool DB::updateRecordInfo(const CrateRecordInfo d)
{
    QString command = QString(
        "UPDATE doklady SET cislo_dokladu = '%1', icp = '%2', odbornost = '%3', var_symbol='%4', rodne_cislo='%5', zakl_diagnoza='%6'"
        " WHERE rowid = '%7'").arg(d.cislo_dokladu, d.icp, d.odb, d.var_symbol, d.rodne_cislo, d.zakl_diag, QString("%1").arg(d.rowid));
    return executeCommand(command) ? true : false;

}

bool DB::changeDiagnose(const QString id_vykonu, const QString s_diag, const QString n_diag)
{
//TODO nazev u funkce
    QString command = QString(
            "UPDATE vykony SET diagnoza = '%1' WHERE id_dokladu='%2' AND diagnoza ='%3'").arg(n_diag, id_vykonu, s_diag);
    return executeCommand(command) ? true : false;
}

int DB::getNumberOfRecords(QString id_davky)
{
    QString command = QString("SELECT pocet_dokladu FROM seznam_davek WHERE id_davky=%1").arg(id_davky);
    QSqlQuery query(command);
    if (checkQuery(query)) {
        if (query.next()) {
            return query.value(0).toInt();
        }
    }
    return 0;
}

QStringList DB::getNumberOfRecordsPoints(QString id_davky)
{
    QString command = QString("SELECT pocet_dokladu, pocet_bodu FROM seznam_davek "
                              "WHERE id_davky=%1").arg(id_davky);
    QStringList d;
    QSqlQuery query(command);
    if (checkQuery(query)) {
        if (query.next()) {
            d << query.value(0).toString() << query.value(1).toString();
        }
    }
    return d;   
}

QStringList DB::getNumberOfPointsServices(qint64 rowid)
{
//TODO rowid
    QString command = QString("SELECT body_celkem, vykony_celkem FROM doklady "
                          "WHERE rowid = %1").arg(rowid);
    QSqlQuery query(command);
    QStringList d;
    if (checkQuery(query)) {
        if (query.next()) {
            d << query.value(0).toString() << query.value(1).toString();
        }
    }
    return d;
}

int DB::getNumberOfServices(qint64 rowid)
{
//TODO rowid
    QString command = QString("SELECT vykony_celkem FROM doklady WHERE rowid = %1").arg(rowid);
    QSqlQuery query(command);
    if (checkQuery(query)) {
        if (query.next()) {
            return query.value(0).toInt();
        }
    }
    return -1;
    
}

bool DB::deleteRecord(qint64 id_dokladu)
{
    QString command = QString("DELETE FROM doklady WHERE id_dokladu = '%1'").arg(id_dokladu);
    return executeCommand(command) ? true : false;
}

bool DB::deletePacient(QString id_pacient)
{
    QString command = QString("DELETE FROM kartoteka WHERE id_pacient = '%1'").arg(id_pacient);
    return executeCommand(command) ? true : false;
}

bool DB::deleteBatch(QString id_davky)
{
    QString command = QString("DELETE FROM seznam_davek WHERE id_davky='%1'").arg(id_davky);
    return executeCommand(command) ? true : false;
}

int DB::isValidCode(QString code)
{
    QString command = QString("SELECT BOD FROM cis_vykony WHERE KOD='%1' LIMIT 1").arg(code);
    QSqlQuery query(command);
    if (checkQuery(query)) {
        if (query.next()) {
            return query.value(0).toInt();  
        }
    }
    return -1;
}

qint64 DB::saveRecord(const QString record_id, const CrateRecordInfoData d)
{
    QString command = QString("INSERT INTO vykony(id_dokladu,datum,kod,pocet,odbornost,diagnoza,body) "
                      "VALUES('%1','%2','%3',%4,'%5','%6',%7)").arg(record_id, d.datum.toString("dd.MM.yyyy"), d.kod, d.pocet, d.odb, d.diag, QString("%1").arg(d.body));
    return getLastId(command);
}

bool DB::updateRecord(const QString record_id, const qint64 rowid, const CrateRecordInfoData d)
{
    //TODO rowid
    QString command = QString("UPDATE vykony SET id_dokladu = '%1',"
                        "datum = '%2',"
                        "kod = '%3',"
                        "pocet = %4,"
                        "odbornost = '%5',"
                        "diagnoza = '%6',"
                        "body = %7 "
                        "WHERE rowid = %8"
                    ).arg(record_id, d.datum.toString("dd.MM.yyyy"), d.kod, d.pocet, d.odb, d.diag, QString("%1").arg(d.body), QString("%1").arg(rowid));
    return executeCommand(command) ? true : false;
}

bool DB::deleteService(qint64 rowid)
{
//TODO rowid
    QString command = QString("DELETE from vykony WHERE rowid = %1").arg(rowid);
    return executeCommand(command) ? true : false;
}

QString DB::getDiagnose(qint64 id_vykonu)
{
   //TODO nazev funkce, rowid
   QString command = QString("SELECT diagnoza FROM vykony WHERE rowid = '%1'").arg(id_vykonu);
    QSqlQuery query(command);
    if (checkQuery(query)) {
        if (query.next()) {
            return query.value(0).toString();
        }
    }
    return QString();
}

bool DB::changeBatchNumber(const QString old_batch, QString old_comp, QString new_batch, QString new_company)
{
    QString command = QString("UPDATE seznam_davek SET cislo_davky = '%1', pojistovna = '%2' "
                              "WHERE cislo_davky = '%3' AND pojistovna = '%4'").arg(new_batch, new_company, old_batch, old_comp);
    return executeCommand(command) ? true : false;
}

bool DB::startTransaction()
{
	if (db.transaction()) {
		qDebug() << "SQLCOMMAND: Transaction starts";
        return true;
    }
    return false;
}

bool DB::commitTransaction()
{
    if (db.commit()) {
        qDebug() << "SQLCOMMAND: Transaction commited";
        return true;
    } else {
        qDebug() << "ERROR: Failed to commit a transaction -> " << db.lastError().text();
        rollbackTransaction();
        return false;
    }
}

bool DB::rollbackTransaction()
{
    if (db.rollback()) {
       qDebug() << "SQLCOMMAND: Transaction rolled back succesfully"; 
       return true;
    } else {
        qDebug() << "ERROR: Failed to rollback a transaction -> " << db.lastError().text();
        return false;
    }
}

bool DB::shiftRecordNumber(int number, QString batch_id)
{
    
    QString command = QString("UPDATE doklady SET cislo_dokladu = cislo_dokladu + %1 "
                              "WHERE id_davky = %2").arg(number).arg(batch_id);
    return executeCommand(command) ? true : false;    
}


bool DB::shiftRecordNumber(int from, int till, int number, QString batch_id)
{
    QString command;
    if (till > 0) { //neposouvam vsechny doklady, ale jen nejaky interval
        command = QString("UPDATE doklady SET cislo_dokladu = cislo_dokladu + %1 "
                          "WHERE cislo_dokladu > %2 AND cislo_dokladu <= %3 AND id_davky = %4"
                         ).arg(number).arg(from).arg(till).arg(batch_id);
    } else { //posouvam vsechny doklady vetsi nez from
        command = QString("UPDATE doklady SET cislo_dokladu = cislo_dokladu + %1 "
                          "WHERE cislo_dokladu > %2 AND id_davky = %3"
                         ).arg(number).arg(from).arg(batch_id);
    }
    return executeCommand(command) ? true : false;
}


bool DB::findPersonalNumber(const QString rc)
{
    QString command = QString("SELECT 'rodne_cislo' FROM kartoteka WHERE rodne_cislo='%1' LIMIT 1").arg(rc);
    return isInDatabase(command) ? true : false;
  
}

QString DB::getPacientName(const QString rc)
{
  QString command = QString("SELECT jmeno FROM kartoteka WHERE rodne_cislo = '%1'").arg(rc);
  QSqlQuery query(command);
  if (checkQuery(query)) {
    if (query.next()) {
      return query.value(0).toString();
    }
  }
  return QString();
}

QString DB::getPacientSurname(const QString rc)
{
  QString command = QString("SELECT prijmeni FROM kartoteka WHERE rodne_cislo = '%1'").arg(rc);
  QSqlQuery query(command);
  if (checkQuery(query)) {
    if (query.next()) {
      return query.value(0).toString();
    }
  }
  return QString();
}

QString DB::getPacientInsComp(const QString rc)
{
  QString command = QString("SELECT pojistovna FROM kartoteka WHERE rodne_cislo = '%1'").arg(rc);
  QSqlQuery query(command);
  if (checkQuery(query)) {
    if (query.next()) {
      return query.value(0).toString();
    }
  }
  return QString();
}


bool DB::changeInsComp(QString rc, QString poj)
{
    // Pri aktualizaci pojistovny se vynuluje datum registrace, aby registrace probehla znovu.
    QString command = QString("UPDATE kartoteka SET pojistovna = '%1', datum_registrace = '' WHERE rodne_cislo = '%2'").arg(poj).arg(rc);
    return executeCommand(command) ? true : false;
}



bool DB::savePacient(const CratePacient p)
{
  
  QString command = QString("INSERT INTO kartoteka (rodne_cislo, jmeno, prijmeni, pojistovna, datum_vlozeni)"
                            "VALUES ('%1', '%2', '%3', '%4', '%5')").arg(p.rc, p.jmeno, p.prijmeni, p.pojistovna,p.vlozeno);
  return executeCommand(command) ? true : false;    
}

QMap<int, int> DB::numberUnregPacients()
{
    QMap<int, int> map;

    QString command = QString("SELECT pojistovna, COUNT(pojistovna) FROM kartoteka WHERE datum_registrace is null or datum_registrace = '' GROUP BY pojistovna");
    QSqlQuery query(command);
    if (checkQuery(query)) {
        while (query.next()) {
            map.insert(query.value(0).toInt(), query.value(1).toInt()); 
        }
    }
    return map;
}

QList<CratePacient> DB::getUnregisteredPacients(const QString in_comp_number)
{
    QList<CratePacient> data;

    QString command = QString("SELECT id_pacient, rodne_cislo, jmeno, prijmeni, datum_vlozeni FROM kartoteka WHERE pojistovna = '%1' AND datum_registrace IS NULL "
                              "OR datum_registrace = ''").arg(in_comp_number);
    QSqlQuery query(command);
    if (checkQuery(query)) {
        while (query.next()) {
            CratePacient d;
            d.id_pacient = query.value(0).toString();
            d.rc= query.value(1).toString();
            d.jmeno = query.value(2).toString();
            d.prijmeni = query.value(3).toString();
            d.vlozeno = query.value(4).toString();
            
            data.append(d);
        }
    }

    return data;
}

void DB::saveUnregisterPacient(QList<CratePacient> list)
{
    QString command;
    for (int i = 0; i < list.size(); ++i) {
        QDate d = QDate::currentDate();
        command = QString("UPDATE kartoteka SET datum_registrace = '%1' WHERE rodne_cislo = '%2'").arg(d.toString("dd.MM.yyyy")).arg(list.at(i).rc);
        executeCommand(command);
   } 
}

CrateRecordInfo::CrateRecordInfo()
{
}

CrateRecordInfo::~CrateRecordInfo()
{
}

bool CrateRecordInfo::isEmpty()
{
    if (id_davky.isEmpty() && cislo_dokladu.isEmpty() && icp.isEmpty() && odb.isEmpty() && var_symbol.isEmpty() && rodne_cislo.isEmpty() && 
        zakl_diag.isEmpty() && body_celkem.isEmpty() && vykony_celkem.isEmpty()){
        return true;
    } 

    return false;

}


