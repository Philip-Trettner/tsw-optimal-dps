#include "SkillTable.hh"

#include "common.hh"

#include <QFile>
#include <QStringList>

#include <QLocale>

std::map<std::string, double> SkillTable::sName2Scaling;
std::map<std::string, std::string> SkillTable::sName2ID;

void SkillTable::loadSkillTable(const string &filename)
{
    QLocale::setDefault(QLocale::English);

    sName2Scaling.clear();
    QFile file(QString::fromStdString(filename));
    if (!file.open(QIODevice::ReadOnly))
    {
        std::cerr << "Unable to open " << filename << std::endl;
        return;
    }

    bool first = true;

    QStringList header;

    int iScaling = -1;
    int iName = -1;

    while (!file.atEnd())
    {
        QString line = file.readLine();
        QStringList parts = line.split("\t");

        if (parts.empty())
            continue; // empty line

        // header
        if (first)
        {
            header = parts;
            iScaling = header.indexOf("Scaling");
            iName = header.indexOf("Name");
            assert(iScaling >= 0);
            assert(iName >= 0);
            first = false;
            continue;
        }

        if (!parts[0].trimmed().isEmpty())
            continue; // special lines

        if (parts.length() > iScaling && parts.length() > iName)
        {
            if (parts[iName].isEmpty() || parts[iScaling].isEmpty())
                continue; // empty name, scaling

            auto name = parts[iName].toStdString();
            auto dmg = parts[iScaling].toDouble();
            if (sName2Scaling.count(name))
            {
                std::cerr << "Double entry for " << name << std::endl;
                assert(0);
            }

            // std::cout << name << " -> " << dmg << std::endl;

            sName2Scaling[name] = dmg;
        }
    }
}

void SkillTable::loadVDMTable(std::string const& filename)
{
    QLocale::setDefault(QLocale::English);

    sName2ID.clear();
    QFile file(QString::fromStdString(filename));
    if (!file.open(QIODevice::ReadOnly))
    {
        std::cerr << "Unable to open " << filename << std::endl;
        return;
    }

    bool first = true;

    QStringList header;

    int iID = -1;
    int iName = -1;

    while (!file.atEnd())
    {
        QString line = file.readLine();
        QStringList parts = line.split("\t");

        if (parts.empty())
            continue; // empty line

                      // header
        if (first)
        {
            header = parts;
            iID = header.indexOf("VDM");
            iName = header.indexOf("Abilite");
            assert(iScaling >= 0);
            assert(iName >= 0);
            first = false;
            continue;
        }

        if (!parts[0].trimmed().isEmpty())
            continue; // special lines

        if (parts.length() > iID && parts.length() > iName)
        {
            if (parts[iName].isEmpty() || parts[iID].isEmpty())
                continue; // empty name, scaling

            auto name = parts[iName].toStdString();
            auto id = parts[iID].toStdString();
            if (sName2ID.count(name))
            {
                std::cerr << "Double entry for " << name << std::endl;
                assert(0);
            }

            // std::cout << name << " -> " << dmg << std::endl;

            sName2ID[name] = id;
        }
    }
}

float SkillTable::scaling(const string &name)
{
    if (!sName2Scaling.count(name))
    {
        std::cerr << "No skill entry found for " << name << std::endl;
        assert(0);
    }

    return (float)sName2Scaling[name];
}
