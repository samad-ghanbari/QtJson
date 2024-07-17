#include <QCoreApplication>
#include <QString>
#include <QVariant>
#include <QFile>
#include <QByteArray>
#include <QTextStream>
#include <QDebug>

//json specific
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>

void writeJsonFile();
void readJsonFile();

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    writeJsonFile();
    readJsonFile();

    return a.exec();
}


void writeJsonFile()
{
    qDebug() << "Write Json File: ";

    //1. Create a QJsonObject and add values to it
    QJsonObject jsonObj;
    jsonObj["FirstName"] = "John";
    //no value will be written as null in the json file
    jsonObj["MiddleName"];
    jsonObj["LastName"] = "Doe";
    jsonObj["Age"] = 43;

    //2. Create Item of Json Object content (object of object)
    QJsonObject jsonItemObj;
    jsonItemObj["Street"] = "Downing Street 10";
    jsonItemObj["City"] = "London";
    jsonItemObj["Country"] = "Great Britain";
    //3. Add jsonItemObj to jsonObj and give it an object Name
    jsonObj["Address"] = jsonItemObj;

    //4. Create jsonArray and fill it with values - similar to filling a vector
    QJsonArray jsonArray;
    jsonArray.append("+44 1234567");
    jsonArray.append("+44 2345678");

    //Add a bool to the Object
    jsonObj["Valid"] = true;

    //5. Add jsonArray to jsonObj and give it an object Name
    jsonObj["Phone numbers"] = jsonArray;
    //(It can also be added to the jsonItemObj to be inline with the Address section)
    //with jsonItemObj["Phone numbers"] = jsonArray or as much objects of objects
    //you need


    /* As I understood it, most Qt classes use a QByteArray to handle data internally
     * because it is really fast/efficient,
     * also QFile QIODevice, it is a good idea to hold the read/write
     * QIODevice data as QByteArray in the Memory
     */

    //6. Create a QByteArray and fill it with QJsonDocument (json formatted)
    QByteArray byteArray;
    byteArray = QJsonDocument(jsonObj).toJson();

    //7. Open a QFile and write the byteArray filled with json formatted data
    //thanks to the QJsonDocument() Class to the file
    QFile file;
    file.setFileName("file.json");
    if(!file.open(QIODevice::WriteOnly)){
        qDebug() << "No write access for json file";
        return;
    }

    //8. finally write the file and close it
    file.write(byteArray);
    file.close();

    //9. Print out the byteArray to the terminal
    QTextStream textStream(stdout);
    textStream << "Rendered json byteArray text: " << endl;
    textStream << byteArray << endl;
}

void readJsonFile()
{
    qDebug() << "Read Json File:";

    //1. Open the QFile and write it to a byteArray and close the file
    QFile file;
    file.setFileName("file.json");
    if(!file.open(QIODevice::ReadOnly)){
        qDebug() << "Json filef couldn't be opened/found";
        return;
    }

    QByteArray byteArray;
    byteArray = file.readAll();
    file.close();

    //2. Format the content of the byteArray as QJsonDocument
    //and check on parse Errors
    QJsonParseError parseError;
    QJsonDocument jsonDoc;
    jsonDoc = QJsonDocument::fromJson(byteArray, &parseError);
    if(parseError.error != QJsonParseError::NoError){
        qWarning() << "Parse error at " << parseError.offset << ":" << parseError.errorString();
        return;
    }

    //3. Create a jsonObject and fill it with the byteArray content, formatted
    //and holding by the jsonDocument Class
    QJsonObject jsonObj;
    jsonObj = jsonDoc.object();

    //4. Now picking the jsonValues and printing them out or do what ever you need
    QJsonValue jsonVal;
    QTextStream textStream(stdout);

    jsonVal = jsonObj.value("FirstName");
    textStream << jsonVal.toString() << endl;

    jsonVal = jsonObj.value("MiddleName");
    //null gets back to an empty fild - added the sting "null/empty" to make it visible
    textStream << jsonVal.toVariant().toString() << "null/empty" << endl;

    jsonVal = jsonObj.value("LastName");
    textStream << jsonVal.toString() << endl;

    //The number has to be converted to an int and than to a string to print it
    jsonVal = jsonObj.value("Age");
    textStream << QString::number(jsonVal.toInt()) << endl;

    //5. Now we need to fill the object of the object. To do that, we need
    //the Item Object and a jsonSubVal object for json without a loop
    QJsonObject jsonItemObj;
    QJsonValue jsonSubVal;

    jsonVal = jsonObj.value(QString("Address"));
    jsonItemObj = jsonVal.toObject();

    jsonSubVal = jsonItemObj["Street"];
    textStream << jsonSubVal.toString() << endl;

    jsonSubVal = jsonItemObj["City"];
    textStream << jsonSubVal.toString() << endl;

    jsonSubVal = jsonItemObj["Country"];
    textStream << jsonSubVal.toString() << endl;

    //6. now the Phone Numbers array with a loop
    QJsonArray jsonArray;
    jsonArray = jsonObj["Phone numbers"].toArray();

    for(int i = 0; i < jsonArray.size(); i++)
        textStream << jsonArray[i].toString() << endl;

    textStream << "or with foreach" << endl;

    foreach(QJsonValue v, jsonArray)
        textStream << v.toString() << endl;

    //7. And finally the bool value:
    jsonVal = jsonObj.value(QString("Valid"));
    textStream << jsonVal.toVariant().toString() << endl;

    textStream << "or as number, not a string: ";

    textStream << (QString::number(jsonVal.toInt())) << endl;

    textStream << "\nThe whole file as printed in the file \n" <<
        jsonDoc.toJson(QJsonDocument::Indented);

}

