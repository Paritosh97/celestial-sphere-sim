/***************************************************************************
                          skypoint.h  -  K Desktop Planetarium
                             -------------------
    begin                : Sun Feb 11 2001
    copyright            : (C) 2001-2005 by Jason Harris
    email                : jharris@30doradus.org
    copyright            : (C) 2004-2005 by Pablo de Vicente
    email                : p.devicente@wanadoo.es
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#pragma once

#include "auxiliary/cachingdms.h"
#include "time/kstarsdatetime.h"

#include <QList>
#include <QtDBus/QtDBus>

//#define PROFILE_COORDINATE_CONVERSION

class KSNumbers;

/**
 * @class SkyPoint
 *
 * The sky coordinates of a point in the sky.  The
 * coordinates are stored in both Equatorial (Right Ascension,
 * Declination) and Horizontal (Azimuth, Altitude) coordinate systems.
 * Provides set/get functions for each coordinate angle, and functions
 * to convert between the Equatorial and Horizon coordinate systems.
 *
 * Because the coordinate values change slowly over time (due to
 * precession, nutation), the "catalog coordinates" are stored
 * (RA0, Dec0), which were the true coordinates on Jan 1, 2000.
 * The true coordinates (RA, Dec) at any other epoch can be found
 * from the catalog coordinates using updateCoords().
 * @short Stores dms coordinates for a point in the sky.
 * for converting between coordinate systems.
 *
 * @author Jason Harris
 * @version 1.0
 */
class SkyPoint
{
    public:
        /**
         * Default constructor: Sets RA, Dec and RA0, Dec0 according
         * to arguments.  Does not set Altitude or Azimuth.
         *
         * @param r Right Ascension
         * @param d Declination
         */
        SkyPoint(const dms &r, const dms &d) : RA0(r), Dec0(d), RA(r), Dec(d), lastPrecessJD(J2000) {}

        SkyPoint(const CachingDms &r, const CachingDms &d) : RA0(r), Dec0(d), RA(r), Dec(d), lastPrecessJD(J2000) {}

        /**
         * Alternate constructor using double arguments, for convenience.
         * It behaves essentially like the default constructor.
         *
         * @param r Right Ascension, expressed as a double
         * @param d Declination, expressed as a double
         * @note This also sets RA0 and Dec0
         */
        //FIXME: this (*15.0) thing is somewhat hacky.
        explicit SkyPoint(double r, double d) : RA0(r * 15.0), Dec0(d), RA(r * 15.0), Dec(d), lastPrecessJD(J2000) {}

        /** @short Default constructor. Sets nonsense values for RA, Dec etc */
        SkyPoint();

        virtual ~SkyPoint() = default;

        ////
        //// 1.  Setting Coordinates
        //// =======================

        /**
         * @short Sets RA, Dec and RA0, Dec0 according to arguments.
         * Does not set Altitude or Azimuth.
         *
         * @param r Right Ascension
         * @param d Declination
         * @note This function also sets RA0 and Dec0 to the same values, so call at your own peril!
         * @note FIXME: This method must be removed, or an epoch argument must be added.
         */
        void set(const dms &r, const dms &d);

        /**
         * Sets RA0, the catalog Right Ascension.
         *
         * @param r catalog Right Ascension.
         */
        inline void setRA0(dms r)
        {
            RA0 = r;
        }
        inline void setRA0(CachingDms r)
        {
            RA0 = r;
        }

        /**
         * Overloaded member function, provided for convenience.
         * It behaves essentially like the above function.
         *
         * @param r Right Ascension, expressed as a double.
         */
        inline void setRA0(double r)
        {
            RA0.setH(r);
        }

        /**
         * Sets Dec0, the catalog Declination.
         *
         * @param d catalog Declination.
         */
        inline void setDec0(dms d)
        {
            Dec0 = d;
        }
        inline void setDec0(const CachingDms &d)
        {
            Dec0 = d;
        }

        /**
         * Overloaded member function, provided for convenience.
         * It behaves essentially like the above function.
         *
         * @param d Declination, expressed as a double.
         */
        inline void setDec0(double d)
        {
            Dec0.setD(d);
        }

        /**
         * Sets RA, the current Right Ascension.
         *
         * @param r Right Ascension.
         */
        inline void setRA(dms &r)
        {
            RA = r;
        }
        inline void setRA(const CachingDms &r)
        {
            RA = r;
        }

        /**
         * Overloaded member function, provided for convenience.
         * It behaves essentially like the above function.
         *
         * @param r Right Ascension, expressed as a double.
         */
        inline void setRA(double r)
        {
            RA.setH(r);
        }

        /**
         * Sets Dec, the current Declination
         *
         * @param d Declination.
         */
        inline void setDec(dms d)
        {
            Dec = d;
        }
        inline void setDec(const CachingDms &d)
        {
            Dec = d;
        }

        /**
         * Overloaded member function, provided for convenience.
         * It behaves essentially like the above function.
         *
         * @param d Declination, expressed as a double.
         */
        inline void setDec(double d)
        {
            Dec.setD(d);
        }

        /**
         * Sets Alt, the Altitude.
         *
         * @param alt Altitude.
         */
        inline void setAlt(dms alt)
        {
            Alt = alt;
        }

        /**
         * Overloaded member function, provided for convenience.
         * It behaves essentially like the above function.
         *
         * @param alt Altitude, expressed as a double.
         */
        inline void setAlt(double alt)
        {
            Alt.setD(alt);
        }

        /**
         * Sets Az, the Azimuth.
         *
         * @param az Azimuth.
         */
        inline void setAz(dms az)
        {
            Az = az;
        }

        /**
         * Overloaded member function, provided for convenience.
         * It behaves essentially like the above function.
         *
         * @param az Azimuth, expressed as a double.
         */
        inline void setAz(double az)
        {
            Az.setD(az);
        }

        ////
        //// 2. Returning coordinates.
        //// =========================

        /** @return a pointer to the catalog Right Ascension. */
        inline const CachingDms &ra0() const
        {
            return RA0;
        }

        /** @return a pointer to the catalog Declination. */
        inline const CachingDms &dec0() const
        {
            return Dec0;
        }

        /** @returns a pointer to the current Right Ascension. */
        inline const CachingDms &ra() const
        {
            return RA;
        }

        /** @return a pointer to the current Declination. */
        inline const CachingDms &dec() const
        {
            return Dec;
        }

        /** @return a pointer to the current Azimuth. */
        inline const dms &az() const
        {
            return Az;
        }

        /** @return a pointer to the current Altitude. */
        inline const dms &alt() const
        {
            return Alt;
        }

        /** @return the JD for the precessed coordinates */
        inline double getLastPrecessJD() const
        {
            return lastPrecessJD;
        }

        /**
         * @return the airmass of the point. Convenience method.
         * @note Question: is it better to use alt or refracted alt? Minor difference, probably doesn't matter.
         */
        inline double airmass() const
        {
            return 1. / sin(alt().radians());
        }

        ////
        //// 3. Coordinate conversions.
        //// ==========================

        /**
         * Determine the (Altitude, Azimuth) coordinates of the
         * SkyPoint from its (RA, Dec) coordinates, given the local
         * sidereal time and the observer's latitude.
         * @param LST pointer to the local sidereal time
         * @param lat pointer to the geographic latitude
         */
        void EquatorialToHorizontal(const CachingDms *LST, const CachingDms *lat);

        // Deprecated method provided for compatibility
        void EquatorialToHorizontal(const dms *LST, const dms *lat);

        /**
         * Determine the (RA, Dec) coordinates of the
         * SkyPoint from its (Altitude, Azimuth) coordinates, given the local
         * sidereal time and the observer's latitude.
         *
         * @param LST pointer to the local sidereal time
         * @param lat pointer to the geographic latitude
         */
        void HorizontalToEquatorial(const dms *LST, const dms *lat);

        /**
         * Determine the Ecliptic coordinates of the SkyPoint, given the Julian Date.
         * The ecliptic coordinates are returned as reference arguments (since
         * they are not stored internally)
         */
        void findEcliptic(const CachingDms *Obliquity, dms &EcLong, dms &EcLat);

        /**
         * Set the current (RA, Dec) coordinates of the
         * SkyPoint, given pointers to its Ecliptic (Long, Lat) coordinates, and
         * to the current obliquity angle (the angle between the equator and ecliptic).
         */
        void setFromEcliptic(const CachingDms *Obliquity, const dms &EcLong, const dms &EcLat);

        /**
         * Computes the angular distance between two SkyObjects. The algorithm
         * to compute this distance is:
         * cos(distance) = sin(d1)*sin(d2) + cos(d1)*cos(d2)*cos(a1-a2)
         * where a1,d1 are the coordinates of the first object and a2,d2 are
         * the coordinates of the second object.
         * However this algorithm is not accurate when the angular separation is small.
         * Meeus provides a different algorithm in page 111 which we implement here.
         *
         * @param sp SkyPoint to which distance is to be calculated
         * @param positionAngle if a non-null pointer is passed, the position angle [E of N]
         * in degrees from this SkyPoint to sp is computed and stored in the passed variable.
         * @return dms angle representing angular separation.
         **/
        dms angularDistanceTo(const SkyPoint *sp, double *const positionAngle = nullptr) const;

        /** @return returns true if _current_ epoch RA / Dec match */
        inline bool operator==(SkyPoint &p) const
        {
            return (ra() == p.ra() && dec() == p.dec());
        }





        static double cpuTime_EqToHz;
        static long unsigned eqToHzCalls;

    protected:
        /**
         * Precess this SkyPoint's catalog coordinates to the epoch described by the
         * given KSNumbers object.
         *
         * @param num pointer to a KSNumbers object describing the target epoch.
         */
        void precess(const KSNumbers *num);

    private:
        CachingDms RA0, Dec0; //catalog coordinates
        CachingDms RA, Dec;   //current true sky coordinates
        dms Alt, Az;


        // long version of these epochs
#define J2000L          2451545.0L   //Julian Date for noon on Jan 1, 2000 (epoch J2000)
#define B1950L          2433282.4235L // Julian date for Jan 0.9235, 1950

    protected:
        double lastPrecessJD { 0 }; // JD at which the last coordinate  (see updateCoords) for this SkyPoint was done
};