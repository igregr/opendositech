#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QMessageBox>
#include <QVariant>
#include <QDate>
#include <QMap>
#include <QSqlRecord>

class CrateRecordInfo
{
public:
    CrateRecordInfo();
    ~CrateRecordInfo();
    bool isEmpty();

    QString id_davky;
    QString cislo_dokladu;

    QString icp;
    QString odb;
    QString var_symbol;
    QString rodne_cislo;
    QString zakl_diag;
    QString body_celkem;
    QString vykony_celkem;
    QString id_vykonu;
    qint64 rowid;   
};

class CrateRecordInfoData
{
public:
    QDate datum;
    QString kod;
    QString pocet;
    QString odb;
    QString diag;
    int body;
    qint64 rowid;
};


class CrateBatch
{
public:
    QString id_davky;
    QString charakter;
    QString typ;
    QString icz;
    QString pob;
    QString rok;
    QString mesic;
    QString cislo_davky;
    QString pojistovna;
    QString pocet_dokladu;
    QString pocet_bodu;
    QString vytvoreni_davky;
};

class CratePacient
{
public:
    QString id_pacient;
    QString rc;
    QString jmeno;
    QString prijmeni;
    QString pojistovna;
    QString vlozeno;
    QString registrovano;
};

enum DiagnoseStatus { DIAGNOSE_OK, DIAGNOSE_BAD, DIAGNOSE_STAR };

enum BatchTable {
    BT_ID_DAVKY,
    BT_CHARAKTER,
    BT_TYP,
    BT_ICZ,
    BT_POB,
    BT_ROK,
    BT_MESIC,
    BT_CISLO_DAVKY,
    BT_POJISTOVNA,
    BT_POCET_DOKLADU,
    BT_POCET_BODU,
    BT_VYTVORENI_DAVKY
};

enum RecordTable {
    RT_ID_DOKLADU,
    RT_ID_DAVKY,
    RT_CISLO_DOKLADU,
    RT_ICP, 
    RT_ODBORNOST,
    RT_VAR_SYMBOL,
    RT_RODNE_CISLO,
    RT_ZAKL_DIAGNOZA,
    RT_BODY_CELKEM,
    RT_VYKONY_CELKEM
};

enum ServiceTable {
    ST_ID_VYKONU,
    ST_ID_DOKLADU,
    ST_DATUM,
    ST_KOD,
    ST_POCET,
    ST_ODBORNOST,
    ST_DIAGNOZA,
    ST_BODY
};

enum RegisterTable {
    R_ID_PACIENT,
    R_RODNE_CISLO,
    R_JMENO,
    R_PRIJMENI,
    R_POJISTOVNA,
    R_DATUM_VLOZENI,
    R_DATUM_REGISTRACE
};


class DB 
{
public:
    static DB* getInstance() { return m_instance; }
    bool init();

    /*
     * Otestuje, zda je hledana pobocka v ciselniku
     * vraci true, pokud je pobocka nalezena
     *
     * parametry:
     *   pob - pobocka
     */
    bool isInsuranceOffice(const QString pob);
    /*
     * Otestuje, zda je hledana pojistovna v ciselniku
     * vrati true, pokud je nalezena
     *
     * parametry:
     *   company - cislo pojistovny
     */
    bool isInsuranceCompany(const QString company);
    
    /*vytvori popis nove davky (nastavi default hodnoty) v tabulce seznam_davek
     * vraci id_davky, ktera byla vytvorena, jinak -1
     */
    qint64 createNewBatchSignature(CrateBatch batch_info);
    
    /*zjisti podle cisla dokladu, jake je pouzito datum v davce
     * napr. 110001 -> listopad (11)
     * 50001 -> kveten (5)
     */
    int getBatchMonth(const QString id_davky);

    /* Zjisti, zda-li kartoteka obsahuje rodne cislo */
    bool findPersonalNumber(const QString rc);

    /* Vrati jmeno, prijmeni pacienta s dotazovanym RC*/
    QString getPacientName(const QString rc);
    QString getPacientSurname(const QString rc);
    QString getPacientInsComp(const QString rc);

    /* Ulozi pacienta do kartoteky */
    bool savePacient(const CratePacient p);

    /* Vrati pocet nezaregistrovanych pocientu u jednotlivych pojistoven */
    QMap<int, int> numberUnregPacients();

    /* Vrati seznam neregistrovanych pacientu dane pojistovny */
    QList<CratePacient> getUnregisteredPacients(const QString in_comp_number);

    void saveUnregisterPacient(QList<CratePacient> list);
    /* Vrati unikatni ID pro davku v danem roce */
    QString getBatchID();

    /* Vrati unikatni ID pro doklady v danem roce */
    QString getRecordID();

    /* Zmeni v kartotece pojistovnu daneho pacienta */
    bool changeInsComp(QString rc, QString poj); 

    /* Smaze v kartotece pacienta s danym ID */
    bool deletePacient(QString id_pacient);

    QString getMaxRecordNumber(const QString id_davky);

    /*
     * Vraci seznam davek z databaze. 
     * Muze vratit vice davek. Cislo davky je unikatni pouze v danem roce, tedy pokud 
     * se za rok pouzije to same, pojistovna by to mela akceptovat
     *
     * parametry:
        batch_number    - cislo hledane davky
        in_comp_number  - pojistovna hledane davky
     *
     */
    QList<CrateBatch> getBatchInfo(const QString batch_number, const QString in_comp_number);

    CrateRecordInfo getRecordInfo(const QString batch_id, const QString record_number);
    QList<CrateRecordInfoData> getRecordData(const QString record_id);

    DiagnoseStatus isValidDiagnose(const QString diag);
    qint64 saveRecordInfo(const CrateRecordInfo d);
    int getNumberOfRecords(QString id_davky);
    int getNumberOfServices(qint64 rowid);
    QString getDiagnose(qint64 id_vykonu);

    QStringList getNumberOfRecordsPoints(QString id_davky);
    QStringList getNumberOfPointsServices(qint64 rowid);

    int isValidCode(QString code);
    qint64 saveRecord(const QString record_id, const CrateRecordInfoData d);
    bool updateRecord(const QString record_id, const qint64 rowid, const CrateRecordInfoData d);
    bool updateRecordInfo(const CrateRecordInfo d);
    bool changeBatchNumber(const QString old_batch, QString in_comp, QString new_batch, QString new_in_comp);
    bool deleteBatch(QString id_davky);
    bool deleteService(qint64 rowid);
    bool deleteRecord(qint64 id_dokladu);

    bool changeDiagnose(const QString id_vykonu, const QString stara_diag, const QString nova_diag);
    void closeDatabase();

    bool startTransaction();
    bool commitTransaction();
    bool rollbackTransaction();

    /*
     * Posune cislo dokladu o pozadovany pocet. Vraci true, pokud se podarilo, false, pokud nastala chyba
     *
     * parametry:
     *   from - od jakeho cisla dokladu posouvame
     *   till - do jakeho cisla dokladu posouvame, pokud je 0, posunou se vsechny doklady vetsi nez from
     *   batch_id - id davky, ve ktere posouvame doklady
     */
    bool shiftRecordNumber(int from, int till, int number, QString batch_id);
    bool shiftRecordNumber(int number, QString batch_id);


private:
    DB();
    DB(const DB&);
    DB& operator=(const DB&);
    ~DB();

    static DB* m_instance;
    
    QSqlDatabase db;

    bool createConnection();
    bool initDatabase();
    bool attachDatabases();
    bool ctSeznamDavek();
    bool ctDoklady();
    bool ctVykony();
    bool ctKartoteka();
    bool ctIDs();
    bool executeCommand(const QString command, bool find = false);
    bool isInDatabase(const QString command);
    bool checkQuery(const QSqlQuery &query);
    qint64 getLastId(QString command);

};



#endif
