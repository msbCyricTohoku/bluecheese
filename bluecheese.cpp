#include "bluecheese.h"
#include "ui_bluecheese.h"
#include <QApplication>
#include <QProcess>
#include <QDebug>
#include <QtDebug>
#include <QFileDialog>                  // file open/save dialogs
#include <QFile>                        // file descriptors, IO
#include <QTextStream>                  // file IO
#include <QStandardPaths>               // default open directory
#include <QMainWindow>
#include <QPushButton>
#include <QMessageBox>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QApplication>
#include <QShortcut>
#include <QStringList>
#include <QCoreApplication>
#include <QString>
#include <QRegularExpression>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <cryptopp/files.h>
#include <cryptopp/osrng.h>
#include <cryptopp/hex.h>
#include <cryptopp/pwdbased.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <cryptopp/osrng.h> // Include for random IV generation
#include <cryptopp/base64.h> // Include for Base64 encoding
#include <cryptopp/base64.h> // Include for Base64 decoding
#include <cryptopp/sha.h>
#include <cryptopp/cryptlib.h> // Include for Crypto++ exceptions


using namespace CryptoPP;
namespace fs = boost::filesystem;


bluecheese::bluecheese(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::bluecheese)
{
    ui->setupUi(this);

    ui->textEdit->setText("enter a secret text here...");

    ui->lineEdit->setText("password here");


}

bluecheese::~bluecheese()
{
    delete ui;
}




void bluecheese::on_pushButton_clicked()
{
    QString plaintext = ui->textEdit->toPlainText(); //"Hello, world!";
    QString passphrase = ui->lineEdit->text(); //"mysecretkey";

    QString passphrasex = ui->lineEdit->text();


    //convert the QString to a std::string for reversal
    std::string passphraseStd = passphrasex.toStdString();

    //reverse the std::string
    std::reverse(passphraseStd.begin(), passphraseStd.end());

    //convert the reversed std::string back to a QString
    passphrasex = QString::fromStdString(passphraseStd);



    QString iv = passphrasex; //"passphrase-to create-a-256-bit-(32-byte)-key-for-AES-256"; // Predefined IV (must be 16 bytes)
    //qDebug() << iv << Qt::endl;

    //hash the passphrase to create a 256-bit (32-byte) key for AES-256
    std::string keyHash;
    CryptoPP::SHA256 hash;
    CryptoPP::StringSource(passphrase.toStdString(), true, new CryptoPP::HashFilter(hash, new CryptoPP::StringSink(keyHash)));

    //convert the key and IV to bytes
    QByteArray keyBytes = QByteArray::fromStdString(keyHash).left(16); // Use the first 16 bytes for AES-128
    QByteArray ivBytes = iv.toUtf8();

    //padding 16 byte
    ivBytes = ivBytes.left(16).rightJustified(16, '\0');

    //set up AES encryption with the derived key and predefined IV based on user passphrase
    AES::Encryption aesEncryption(reinterpret_cast<const byte*>(keyBytes.data()), CryptoPP::AES::DEFAULT_KEYLENGTH);
    CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, reinterpret_cast<const byte*>(ivBytes.data()));

    QByteArray plaintextBytes = plaintext.toUtf8();
    std::string ciphertext;
    CryptoPP::StringSource(reinterpret_cast<const byte*>(plaintextBytes.data()), plaintextBytes.size(), true,
                           new CryptoPP::StreamTransformationFilter(cbcEncryption,
                                                                    new CryptoPP::Base64Encoder(new CryptoPP::StringSink(ciphertext), false) // Base64 encode
                                                                    )
                           );

    //convert the ciphertext to a QString
    QString ciphertextString = QString::fromStdString(ciphertext);


   // qDebug() << "Ciphertext: " << ciphertextString;

    //display the ciphertext in the text browser
    ui->textBrowser->setText(ciphertextString);
}



void bluecheese::on_pushButton_3_clicked()
{
    QString passphrase = ui->lineEdit->text(); //"mysecretkey";

    QString passphrasex = ui->lineEdit->text();


    std::string passphraseStd = passphrasex.toStdString();

    std::reverse(passphraseStd.begin(), passphraseStd.end());


    passphrasex = QString::fromStdString(passphraseStd);

    QString iv = passphrasex;
    //qDebug() << iv << Qt::endl;

    std::string keyHash;
    CryptoPP::SHA256 hash;
    CryptoPP::StringSource(passphrase.toStdString(), true, new CryptoPP::HashFilter(hash, new CryptoPP::StringSink(keyHash)));

    //convert the key and IV to bytes
    QByteArray keyBytes = QByteArray::fromStdString(keyHash).left(16); // Use the first 16 bytes for AES-128
    QByteArray ivBytes = iv.toUtf8();


    ivBytes = ivBytes.left(16).rightJustified(16, '\0');

    //get the ciphertext from the text input
    QString ciphertextString = ui->textEdit->toPlainText();

    //convert the ciphertext to a std::string
    std::string ciphertext = ciphertextString.toStdString();

    //decode the Base64 encoded ciphertext
    std::string decodedCiphertext;
    CryptoPP::StringSource(ciphertext, true,
                           new CryptoPP::Base64Decoder(
                               new CryptoPP::StringSink(decodedCiphertext)
                               )
                           );

    try {

        CryptoPP::AES::Decryption aesDecryption(reinterpret_cast<const byte*>(keyBytes.data()), CryptoPP::AES::DEFAULT_KEYLENGTH);
        CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, reinterpret_cast<const byte*>(ivBytes.data()));


        std::string decryptedtext;
        CryptoPP::StringSource(decodedCiphertext, true,
                               new CryptoPP::StreamTransformationFilter(cbcDecryption,
                                                                        new CryptoPP::StringSink(decryptedtext)
                                                                        )
                               );

        QString decryptedString = QString::fromStdString(decryptedtext);

        ui->textBrowser->setText(decryptedString);

    } catch (const CryptoPP::InvalidCiphertext &e) {
        //handle the error when decryption fails due to an invalid key
        QMessageBox::warning(this, "Decipher Error", "Invalid key or corrupted ciphertext. Decipher failed.");
        ui->textBrowser->clear(); //clear the output if decryption fails
    } catch (const CryptoPP::Exception &e) {
        //here we handle any other CryptoPP exceptions
        QMessageBox::warning(this, "Decryption Error", QString::fromStdString(e.what()));
        ui->textBrowser->clear(); //clear the output if decryption fails
    }
}


void bluecheese::on_pushButton_2_clicked()
{
    QString filename = "saved.txt";
    QString filepath = QFileDialog::getSaveFileName(this, "Save " + filename, "./" + filename);
    QFile file(filepath);
    if (file.open(QIODevice::WriteOnly)) {
        QString changedcode = ui->textBrowser->toPlainText();
        QTextStream out15(&file);
        out15 << changedcode;
        file.close();
    }
}


void bluecheese::on_pushButton_4_clicked()
{
    QApplication::quit();
}


void bluecheese::on_pushButton_5_clicked()
{
QMessageBox msgAbout;
         msgAbout.setInformativeText("<span style='text-align: center'><p><b><font size = 10>Bluecheese</font><p><font size = 5>Text cipher/decipher</font>");
        QPixmap pixAbout;
        pixAbout.load("resources/bluecheese.png");
        msgAbout.setIconPixmap(pixAbout);
        msgAbout.setStandardButtons(QMessageBox::Close);
        msgAbout.setDefaultButton(QMessageBox::Close);
        msgAbout.exec();
}


void bluecheese::on_actionCipher_triggered()
{
    QString plaintext = ui->textEdit->toPlainText(); //"Hello, world!";
    QString passphrase = ui->lineEdit->text(); //"mysecretkey";

    QString passphrasex = ui->lineEdit->text();


    //convert the QString to a std::string for reversal
    std::string passphraseStd = passphrasex.toStdString();

    //reverse the std::string
    std::reverse(passphraseStd.begin(), passphraseStd.end());

    //convert the reversed std::string back to a QString
    passphrasex = QString::fromStdString(passphraseStd);



    QString iv = passphrasex; //"passphrase-to create-a-256-bit-(32-byte)-key-for-AES-256"; // Predefined IV (must be 16 bytes)
    //qDebug() << iv << Qt::endl;

    //hash the passphrase to create a 256-bit (32-byte) key for AES-256
    std::string keyHash;
    CryptoPP::SHA256 hash;
    CryptoPP::StringSource(passphrase.toStdString(), true, new CryptoPP::HashFilter(hash, new CryptoPP::StringSink(keyHash)));

    //convert the key and IV to bytes
    QByteArray keyBytes = QByteArray::fromStdString(keyHash).left(16); // Use the first 16 bytes for AES-128
    QByteArray ivBytes = iv.toUtf8();

    //padding 16 byte
    ivBytes = ivBytes.left(16).rightJustified(16, '\0');

    //set up AES encryption with the derived key and predefined IV based on user passphrase
    AES::Encryption aesEncryption(reinterpret_cast<const byte*>(keyBytes.data()), CryptoPP::AES::DEFAULT_KEYLENGTH);
    CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, reinterpret_cast<const byte*>(ivBytes.data()));

    QByteArray plaintextBytes = plaintext.toUtf8();
    std::string ciphertext;
    CryptoPP::StringSource(reinterpret_cast<const byte*>(plaintextBytes.data()), plaintextBytes.size(), true,
                           new CryptoPP::StreamTransformationFilter(cbcEncryption,
                                                                    new CryptoPP::Base64Encoder(new CryptoPP::StringSink(ciphertext), false) // Base64 encode
                                                                    )
                           );

    //convert the ciphertext to a QString
    QString ciphertextString = QString::fromStdString(ciphertext);

    // Display the ciphertext
    // qDebug() << "Ciphertext: " << ciphertextString;

    //display the ciphertext in the text browser
    ui->textBrowser->setText(ciphertextString);
}


void bluecheese::on_actionDecipher_triggered()
{
    QString passphrase = ui->lineEdit->text(); //"mysecretkey";

    QString passphrasex = ui->lineEdit->text();


    std::string passphraseStd = passphrasex.toStdString();

    std::reverse(passphraseStd.begin(), passphraseStd.end());


    passphrasex = QString::fromStdString(passphraseStd);

    QString iv = passphrasex;
    //qDebug() << iv << Qt::endl;

    std::string keyHash;
    CryptoPP::SHA256 hash;
    CryptoPP::StringSource(passphrase.toStdString(), true, new CryptoPP::HashFilter(hash, new CryptoPP::StringSink(keyHash)));

    //convert the key and IV to bytes
    QByteArray keyBytes = QByteArray::fromStdString(keyHash).left(16); // Use the first 16 bytes for AES-128
    QByteArray ivBytes = iv.toUtf8();


    ivBytes = ivBytes.left(16).rightJustified(16, '\0');

    //get the ciphertext from the text input
    QString ciphertextString = ui->textEdit->toPlainText();

    //convert the ciphertext to a std::string
    std::string ciphertext = ciphertextString.toStdString();

    //decode the Base64 encoded ciphertext
    std::string decodedCiphertext;
    CryptoPP::StringSource(ciphertext, true,
                           new CryptoPP::Base64Decoder(
                               new CryptoPP::StringSink(decodedCiphertext)
                               )
                           );

    try {

        CryptoPP::AES::Decryption aesDecryption(reinterpret_cast<const byte*>(keyBytes.data()), CryptoPP::AES::DEFAULT_KEYLENGTH);
        CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, reinterpret_cast<const byte*>(ivBytes.data()));


        std::string decryptedtext;
        CryptoPP::StringSource(decodedCiphertext, true,
                               new CryptoPP::StreamTransformationFilter(cbcDecryption,
                                                                        new CryptoPP::StringSink(decryptedtext)
                                                                        )
                               );

        QString decryptedString = QString::fromStdString(decryptedtext);

        ui->textBrowser->setText(decryptedString);

    } catch (const CryptoPP::InvalidCiphertext &e) {
        //handle the error when decryption fails due to an invalid key
        QMessageBox::warning(this, "Decipher Error", "Invalid key or corrupted ciphertext. Decipher failed.");
        ui->textBrowser->clear(); //clear the output if decryption fails
    } catch (const CryptoPP::Exception &e) {
        //here we handle any other CryptoPP exceptions
        QMessageBox::warning(this, "Decryption Error", QString::fromStdString(e.what()));
        ui->textBrowser->clear(); //clear the output if decryption fails
    }
}


void bluecheese::on_actionSave_triggered()
{
    QString filename = "saved.txt";
    QString filepath = QFileDialog::getSaveFileName(this, "Save " + filename, "./" + filename);
    QFile file(filepath);
    if (file.open(QIODevice::WriteOnly)) {
        QString changedcode = ui->textBrowser->toPlainText();
        QTextStream out15(&file);
        out15 << changedcode;
        file.close();
    }
}


void bluecheese::on_actionAbout_triggered()
{
    QMessageBox msgAbout;
    msgAbout.setInformativeText("<span style='text-align: center'><p><b><font size = 10>Bluecheese</font><p><font size = 5>Text cipher/decipher</font>");
    QPixmap pixAbout;
    pixAbout.load("resources/bluecheese.png");
    msgAbout.setIconPixmap(pixAbout);
    msgAbout.setStandardButtons(QMessageBox::Close);
    msgAbout.setDefaultButton(QMessageBox::Close);
    msgAbout.exec();
}


void bluecheese::on_actionQuit_triggered()
{
    QApplication::quit();
}

