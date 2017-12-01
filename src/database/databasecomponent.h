#ifndef DATABASECOMPONENT_H
#define DATABASECOMPONENT_H

#include <QObject>
#include <QVariant>
#include <QVector>
#include <QThread>

class Database;

namespace Private {
/**
 * @brief The AsyncSelect class
 * Execute a asynchronous selection in SQLITE database, using 'tableName', 'where' condition and 'args' as selection properties, like 'LIMIT' and 'OFFSET'.
 * The jsonColumns is needed if the table contains columns with serialized json strings, to the value are returned as QVariant(Map/List).
 * To retrieve the result set, the pointer send a 'itemLoaded' signal for each entry, with a QVariantMap as column_name->value.
 */
class AsyncSelect : public QThread {
    Q_OBJECT
public:
    /**
     * @brief AsyncSelect
     * @param tableName QString* the table name to get the query selection
     * @param jsonColumns QStringList* a list of columns names that has serialized json string to parse for map or array
     * @param where QVariantMap a map with columns_name->value to build the query filter condition
     * @param args the selection args to customize the query predicates
     * @param parent QObject* the object parent
     */
    explicit AsyncSelect(QString *tableName, QStringList *jsonColumns, const QVariantMap &where, const QVariantMap &args, QObject *parent = nullptr);

signals:
    /**
     * @brief itemLoaded
     * This signal will be emitted after load some database data (from select method).
     * @param data QVariantMap a map with column_name->value loaded from database.
     */
    void itemLoaded(const QVariantMap &data);

protected:
    /**
     * @brief run
     * @overload
     * The start point for the thread.
     * After call the thread->start(), the newly created thread calls this function.
     * @return void
     */
    void run() override;

private:
    /**
     * @brief m_tableName
     * The name of the table to select in Database
     */
    QString *m_tableName;

    /**
     * @brief m_jsonColumns
     * The list of json columnns from current table, to be serialized as string before save
     * and parse to object or array before sent in 'itemLoaded' signal when select data.
     */
    QStringList *m_jsonColumns;

    /**
     * @brief m_where
     * The predicates filter as object (javascript from QML),
     * the values needs to be: column_name->value.
     */
    QVariantMap m_where;

    /**
     * @brief m_args
     * The selection operators and custom arguments as object, like
     * the operator to compare, the limit of items or offset value (useful when paginate results).
     */
    QVariantMap m_args;

    /**
     * @brief m_database
     * A pointer to Database object where execute the selection.
     */
    Database *m_database;
};
}

/**
 * @brief The DatabaseComponent class
 *
 * This class encapsulate a database operations to application plugins as custom QML type,
 * registered in application Context to be used by QML objects.
 * Uses the singleton instance of Database class to call the hight level methods: select, insert, update and remove.
 *
 * @WARNING!
 * The sqlite database will be created only if any plugin has a file "plugin_table.sql" (with table creation query)
 * in plugin directory. So, the qml object plugin needs to import the "Database" component to
 * use the methods and execute queries, saving or load data from your table.
 * Example:
 * # suposes a file MessagesPage.qml
 * 
 * import QtQuick 2.9
 * import Database 1.0
 *
 * Item {
 *    id: rootItem
 *
 *    Component.onCompleted: database.select({}) // pass a empty predicated
 *
 *    Database {
 *       id: database
 *       jsonColumns: ["sender"] 
 *       onItemLoaded: listViewModel.append(data)
 *    }
 * }
 * 
 * To select uses: select("plugin_table", {"id": 1}). The result wil be sent in itemLoaded signal with a QVariantList of QVariantMap's.
 * To insert uses: insert("plugin_table", {"name": "Mouse Logitech MA1x", "price": 19,55})
 * To update uses: update("plugin_table", {"price": 21,15}, {"name": "Mouse Logitech MA1x"})
 * To remove uses: remove("plugin_table", {"name": "Mouse Logitech MA1x"})
 */
class DatabaseComponent : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int totalItens MEMBER m_totalItens CONSTANT FINAL)
    Q_PROPERTY(QString tableName MEMBER m_tableName WRITE setTableName)
    Q_PROPERTY(QStringList jsonColumns MEMBER m_jsonColumns WRITE setJsonColumns)
public:
    /**
     * @brief DatabaseComponent
     * @param parent QObject* the parent of this object if exists
     */
    explicit DatabaseComponent(QObject *parent = nullptr);

    /**
     * @brief containsId
     * Check if a value for 'id' parameter exists in m_savedIds property.
     * @param id int the ID to check if already exists on database
     * @return bool
     */
    Q_INVOKABLE bool containsId(int id);

    /**
     * @brief insert
     * @param data
     * @return int id of inserted row
     */
    Q_INVOKABLE int insert(const QVariantMap &data);

    /**
     * @brief select
     * @param where QVariantMap
     * @param args QVariantMap
     */
    Q_INVOKABLE void select(const QVariantMap &where, const QVariantMap &args = QVariantMap());

    /**
     * @brief update
     * @param data QVariantMap
     * @param where QVariantMap
     * @return int the number rows updated
     */
    Q_INVOKABLE int update(const QVariantMap &data, const QVariantMap &where);

    /**
     * @brief remove
     * Remove a entries of the table using the where map as filter condition,
     * the where needs to contains the column_name->value to be compared in database.
     * @param where QVariantMap
     * @return int the total number of deleted rows
     */
    Q_INVOKABLE int remove(const QVariantMap &where);

private:
    /**
     * @brief load
     * Load table columns and the total itens saved in database for current plugin
     */
    void load();

    /**
     * @brief setTablename
     * Set in object m_tableName the value for current plugin table name.
     * After set the parameter to m_tableName, if is not empty, call load() method.
     * @param tableName QString
     */
    void setTableName(const QString &tableName);

    /**
     * @brief setJsonColumns
     * Set in object m_jsonColumns the columns
     * @param jsonColumns QStringList QML can set as javascript array
     */
    void setJsonColumns(const QStringList &jsonColumns);

    /**
     * @brief parseData
     * This method parse a object or array to be saved as string serialized in database.
     * Basically, iterate in 'm_jsonColumns' parsing json object or array found in &insertData using QJsonDocument.
     * Uses QMetaType to check each column value type.
     * @param insertData QVariantMap*
     */
    void parseData(QVariantMap *data);

signals:
    /**
     * @brief itemLoaded
     * This signal will be emitted after load some database data in select method.
     * @param data QVariantMap a map with column_name->value loaded from database.
     */
    void itemLoaded(const QVariantMap &data);

private:
    /**
     * @brief m_totalItens
     * A integer with total itens registered in database for current table.
     * It's useful to paginate the entries in database for current table.
     */
    int m_totalItens;

    /**
     * @brief m_savedIds
     * A list of integers with all ID's saved in database for current table.
     * It's useful to check if the current table contains a some item and prevent to insert again.
     */
    QVector<int> m_savedIds;

    /**
     * @brief m_database
     * A pointer to Database object.
     */
    Database *m_database;

    /**
     * @brief m_tableName
     * A string with the current table name.
     * This component is configured to execute operations in one table.
     * To execute in multiple tables, needs to set the table name before call insert, select, remove or update.
     */
    QString m_tableName;

    /**
     * @brief m_tableColumns
     * A string list with current table columns names and is defined internaly using m_database->tableColumns(m_tableName).
     * The columns names is used to parse values for each insert and update data, removing properties that not exists in current table.
     * from objects loaded from webservice.
     */
    QStringList m_tableColumns;

    /**
     * @brief m_jsonColumns
     * A string list with json columns names, saved as serialized strings.
     * m_jsonColumns is used to parse these strings to QVariant(Map/List) before return to QML objects.
     */
    QStringList m_jsonColumns;
};

#endif // DATABASECOMPONENT_H