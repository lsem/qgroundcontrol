/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "AirspaceFlightPlanProvider.h"
#include "AirMapFlightPlanManager.h"
#include "AirMapVehicleManager.h"
#include "AirMapManager.h"

#include "QGCApplication.h"
#include "Vehicle.h"

//-----------------------------------------------------------------------------
AirMapVehicleManager::AirMapVehicleManager(AirMapSharedState& shared, const Vehicle& vehicle)
    : AirspaceVehicleManager(vehicle)
    , _shared(shared)
    , _flightManager(shared)
    , _telemetry(shared)
    , _trafficMonitor(shared)
{
    connect(&_flightManager,  &AirMapFlightManager::error,                      this, &AirMapVehicleManager::error);
    connect(&_telemetry,      &AirMapTelemetry::error,                          this, &AirMapVehicleManager::error);
    connect(&_trafficMonitor, &AirMapTrafficMonitor::error,                     this, &AirMapVehicleManager::error);
    connect(&_trafficMonitor, &AirMapTrafficMonitor::trafficUpdate,             this, &AirspaceVehicleManager::trafficUpdate);
}

//-----------------------------------------------------------------------------
void
AirMapVehicleManager::startTelemetryStream()
{
    AirMapFlightPlanManager* planMgr = (AirMapFlightPlanManager*)qgcApp()->toolbox()->airspaceManager()->flightPlan();
    if (!planMgr->flightID().isEmpty()) {
        //-- TODO: This will start telemetry using the current flight ID in memory (current flight in AirMapFlightPlanManager)
        qCDebug(AirMapManagerLog) << "AirMap telemetry stream enabled";
        _telemetry.startTelemetryStream(planMgr->flightID());
    } else {
        qCDebug(AirMapManagerLog) << "AirMap telemetry stream not possible (No Flight ID)";
    }
}

//-----------------------------------------------------------------------------
void
AirMapVehicleManager::stopTelemetryStream()
{
    _telemetry.stopTelemetryStream();
}

//-----------------------------------------------------------------------------
bool
AirMapVehicleManager::isTelemetryStreaming()
{
    return _telemetry.isTelemetryStreaming();
}

//-----------------------------------------------------------------------------
void
AirMapVehicleManager::endFlight()
{
    AirMapFlightPlanManager* planMgr = (AirMapFlightPlanManager*)qgcApp()->toolbox()->airspaceManager()->flightPlan();
    if (!planMgr->flightID().isEmpty()) {
        _flightManager.endFlight(planMgr->flightID());
    }
    _trafficMonitor.stop();
}

//-----------------------------------------------------------------------------
void
AirMapVehicleManager::vehicleMavlinkMessageReceived(const mavlink_message_t& message)
{
    if (isTelemetryStreaming()) {
        _telemetry.vehicleMessageReceived(message);
    }
}
