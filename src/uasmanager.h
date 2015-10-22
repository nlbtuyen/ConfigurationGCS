#ifndef _UASMANAGER_H_
#define _UASMANAGER_H_

#include <QThread>
#include <QList>
#include <QMutex>
#include "uasinterface.h"
#include "eigen/Eigen/Eigen"

#define DEG2RAD (M_PI/180.0)

#ifndef sincos
#define sincos(th,x,y) { (*(x))=sin(th); (*(y))=cos(th); }
#endif

/**
 *  Central manager for all connected aerial vehicles
 *
 * This class keeps a list of all connected / configured UASs. It also stores which
 * UAS is currently select with respect to user input or manual controls.
 **/
class UASManager : public QObject
{
    Q_OBJECT

public:
    static UASManager* instance();
    ~UASManager();

    /**
     *  Get the currently selected UAS
     *
     * @return NULL pointer if no UAS exists, active UAS else
     **/
    UASInterface* getActiveUAS();
    UASInterface* silentGetActiveUAS();
    /**
     *  Get the UAS with this id
     *
     * Although not enforced by this implementation, the IDs are constrained to be
     * in the range of 1 - 127 by the MAVLINK protocol.
     *
     * @param id unique system / aircraft id
     * @return UAS with the given ID, NULL pointer else
     **/
    UASInterface* getUASForId(int id);

    QList<UASInterface*> getUASList();
    /**  Get home position latitude */
    double getHomeLatitude() const {
        return homeLat;
    }
    /**  Get home position longitude */
    double getHomeLongitude() const {
        return homeLon;
    }
    /**  Get home position altitude */
    double getHomeAltitude() const {
        return homeAlt;
    }

    /**  Get the home position coordinate frame */
    int getHomeFrame() const
    {
        return homeFrame;
    }

    /**  Get home position altitude */
    int getSystemsCount() const {
        return systems.size();
    }

    /**  Convert WGS84 coordinates to earth centric frame */
    Eigen::Vector3d wgs84ToEcef(const double & latitude, const double & longitude, const double & altitude);
    /**  Convert earth centric frame to EAST-NORTH-UP frame (x-y-z directions */
    Eigen::Vector3d ecefToEnu(const Eigen::Vector3d & ecef);
    /**  Convert WGS84 lat/lon coordinates to carthesian coordinates with home position as origin */
    void wgs84ToEnu(const double& lat, const double& lon, const double& alt, double* east, double* north, double* up);
    /**  Convert x,y,z coordinates to lat / lon / alt coordinates in east-north-up frame */
    void enuToWgs84(const double& x, const double& y, const double& z, double* lat, double* lon, double* alt);
    /**  Convert x,y,z coordinates to lat / lon / alt coordinates in north-east-down frame */
    void nedToWgs84(const double& x, const double& y, const double& z, double* lat, double* lon, double* alt);

    void getLocalNEDSafetyLimits(double* x1, double* y1, double* z1, double* x2, double* y2, double* z2)
    {
        *x1 = nedSafetyLimitPosition1.x();
        *y1 = nedSafetyLimitPosition1.y();
        *z1 = nedSafetyLimitPosition1.z();

        *x2 = nedSafetyLimitPosition2.x();
        *y2 = nedSafetyLimitPosition2.y();
        *z2 = nedSafetyLimitPosition2.z();
    }

    /**  Check if a position is in the local NED safety limits */
    bool isInLocalNEDSafetyLimits(double x, double y, double z)
    {
        if (x < nedSafetyLimitPosition1.x() &&
            y > nedSafetyLimitPosition1.y() &&
            z < nedSafetyLimitPosition1.z() &&
            x > nedSafetyLimitPosition2.x() &&
            y < nedSafetyLimitPosition2.y() &&
            z > nedSafetyLimitPosition2.z())
        {
            // Within limits
            return true;
        }
        else
        {
            // Not within limits
            return false;
        }
    }

//    void wgs84ToNed(const double& lat, const double& lon, const double& alt, double* north, double* east, double* down);


public slots:

    /**
     *  Add a new UAS to the list
     *
     * This command will only be executed if this UAS does not yet exist.
     * @param UAS unmanned air system to add
     **/
    void addUAS(UASInterface* UAS);

    /**  Remove a system from the list */
    void removeUAS(UASInterface* uas);


    /**
      *  Set a UAS as currently selected
      *
      * @param UAS Unmanned Air System to set
      **/
    void setActiveUAS(UASInterface* UAS);


    /**
     *  Launch the active UAS
     *
     * The groundstation has always one Unmanned Air System selected.
     * All commands are executed on the UAS in focus. This command starts
     * the launch sequence.
     *
     * @return True if the UAS could be launched, false else
     */
    bool launchActiveUAS();

    bool haltActiveUAS();

    bool continueActiveUAS();

    /**
     *  Land the active UAS
     *
     * The groundstation has always one Unmanned Air System selected.
     * All commands are executed on the UAS in focus. This command starts
     * the land sequence. Depending on the onboard control, this could mean
     * returning to the landing spot as well as descending on the current
     * position.
     *
     * @return True if the UAS could be landed, false else
     */
    bool returnActiveUAS();


    /**
     *  EMERGENCY: Stop active UAS
     *
     * The groundstation has always one Unmanned Air System selected.
     * All commands are executed on the UAS in focus. This command
     * starts an emergency landing. Depending on the onboard control,
     * this usually means descending rapidly on the current position.
     *
     * @warning This command can severely damage the UAS!
     *
     * @return True if the UAS could be landed, false else
     */
    bool stopActiveUAS();

    /**
     *  EMERGENCY: Kill active UAS
     *
     * The groundstation has always one Unmanned Air System selected.
     * All commands are executed on the UAS in focus. This command
     * shuts off all onboard motors immediately. This leads to a
     * system crash, but might prevent external damage, e.g. to people.
     * This command is secured by an additional popup message window.
     *
     * @warning THIS COMMAND RESULTS IN THE LOSS OF THE SYSTEM!
     *
     * @return True if the UAS could be landed, false else
     */
    bool killActiveUAS();

    /**
     *  Configure the currently active UAS
     *
     * This command will bring up the configuration dialog for the particular UAS.
     */
    void configureActiveUAS();

    /**  Shut down the onboard operating system down */
    bool shutdownActiveUAS();

    /**  Set the current home position on all UAVs*/
    bool setHomePosition(double lat, double lon, double alt);

    /**  Set the safety limits in local position frame */
    void setLocalNEDSafetyBorders(double x1, double y1, double z1, double x2, double y2, double z2);

    /**  Update home position based on the position from one of the UAVs */
    void uavChangedHomePosition(int uav, double lat, double lon, double alt);

    /**  Load settings */
    void loadSettings();
    /**  Store settings */
    void storeSettings();


protected:
    UASManager();
    QList<UASInterface*> systems;
    UASInterface* activeUAS;
    QMutex activeUASMutex;
    double homeLat;
    double homeLon;
    double homeAlt;
    int homeFrame;
    Eigen::Quaterniond ecef_ref_orientation_;
    Eigen::Vector3d ecef_ref_point_;
    Eigen::Vector3d nedSafetyLimitPosition1;
    Eigen::Vector3d nedSafetyLimitPosition2;

    void initReference(const double & latitude, const double & longitude, const double & altitude);

signals:

    /** A new system was created */
    void UASCreated(UASInterface* UAS);
    /** A system was deleted */
    void UASDeleted(UASInterface* UAS);
    /**  The UAS currently under main operator control changed */
    void activeUASSet(UASInterface* UAS);
    /**  The UAS currently under main operator control changed */
    void activeUASSet(int systemId);
    /**  The UAS currently under main operator control changed */
    void activeUASSetListIndex(int listIndex);
    /**  The UAS currently under main operator control changed */
    void activeUASStatusChanged(UASInterface* UAS, bool active);
    /**  The UAS currently under main operator control changed */
    void activeUASStatusChanged(int systemId, bool active);
    /**  Current home position changed */
    void homePositionChanged(double lat, double lon, double alt);
public:
    /* Need to align struct pointer to prevent a memory assertion:
     * See http://eigen.tuxfamily.org/dox-devel/TopicUnalignedArrayAssert.html
     * for details
     */
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};


#endif // _UASMANAGER_H_
