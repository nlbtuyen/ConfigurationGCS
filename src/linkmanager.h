#ifndef _LINKMANAGER_H_
#define _LINKMANAGER_H_

#include <QThread>
#include <QList>
#include <QMultiMap>
#include "linkinterface.h"
#include "protocolinterface.h"

/**
 * The Link Manager organizes the physical Links. It can manage arbitrary
 * links and takes care of connecting them as well assigning the correct
 * protocol instance to transport the link data into the application.
 *
 **/
class LinkManager : public QObject
{
    Q_OBJECT

public:
    static LinkManager* instance();
    ~LinkManager();

    void run();

    QList<LinkInterface *> getLinksForProtocol(ProtocolInterface* protocol);
    QList<LinkInterface *> getLinksForType(const int linkType);

    /**  Get the link for this id */
    LinkInterface* getLinkForId(int id);

    /**  Get a list of all links */
    const QList<LinkInterface*> getLinks();

    /**  Get a list of all protocols */
    const QList<ProtocolInterface*> getProtocols() {
        return protocolLinks.uniqueKeys();
    }

public slots:

    void add(LinkInterface* link);
    void addProtocol(LinkInterface* link, ProtocolInterface* protocol);

    void removeLink(QObject* link);
    bool removeLink(LinkInterface* link);

    bool connectAll();
    bool connectLink(LinkInterface* link);

    bool disconnectAll();
    bool disconnectLink(LinkInterface* link);

protected:
    LinkManager();
    QList<LinkInterface*> links;
    QMultiMap<ProtocolInterface*,LinkInterface*> protocolLinks;
    QMultiMap<int, LinkInterface*> linkTypesMap;

private:
    static LinkManager* _instance;

signals:
    void newLink(LinkInterface* link);
    void linkRemoved(LinkInterface* link);

};


#endif // _LINKMANAGER_H_
