#include "skypoint.h"

#include "auxiliary/dms.h"
#include "ksnumbers.h"
#include "time/kstarsdatetime.h"

#include <QDebug>

#include <cmath>

#include <ctime> // For profiling, remove if not profiling.
long unsigned SkyPoint::eqToHzCalls = 0;
double SkyPoint::cpuTime_EqToHz     = 0.;

SkyPoint::SkyPoint()
{
    // Default constructor. Set nonsense values
    RA0.setD(-1);   // RA >= 0 always :-)
    Dec0.setD(180); // Dec is between -90 and 90 Degrees :-)
    RA            = RA0;
    Dec           = Dec0;
    lastPrecessJD = J2000; // By convention, we use J2000 coordinates
}

void SkyPoint::set(const dms &r, const dms &d)
{
    RA0 = RA = r;
    Dec0 = Dec    = d;
    lastPrecessJD = J2000; // By convention, we use J2000 coordinates
}

void SkyPoint::EquatorialToHorizontal(const dms *LST, const dms *lat)
{
    //    qDebug() << "NOTE: This EquatorialToHorizontal overload (using dms pointers instead of CachingDms pointers) is deprecated and should be replaced with CachingDms prototype wherever speed is desirable!";
    CachingDms _LST(*LST), _lat(*lat);
    EquatorialToHorizontal(&_LST, &_lat);
}

void SkyPoint::EquatorialToHorizontal(const CachingDms *LST, const CachingDms *lat)
{
    std::clock_t start = std::clock();
    //Uncomment for spherical trig version
    double AltRad, AzRad;
    double sindec, cosdec, sinlat, coslat, sinHA, cosHA;
    double sinAlt, cosAlt;

    CachingDms HourAngle =
        (*LST) - ra(); // Using CachingDms subtraction operator to find cos/sin of HourAngle without calling sincos()

    lat->SinCos(sinlat, coslat);
    dec().SinCos(sindec, cosdec);
    HourAngle.SinCos(sinHA, cosHA);

    sinAlt = sindec * sinlat + cosdec * coslat * cosHA;
    AltRad = asin(sinAlt);

    cosAlt = sqrt(
                 1 -
                 sinAlt *
                 sinAlt); // Avoid trigonometric function. Return value of asin is always in [-pi/2, pi/2] and in this domain cosine is always non-negative, so we can use this.
    if (cosAlt == 0.)
        cosAlt = cos(AltRad);

    double arg = (sindec - sinlat * sinAlt) / (coslat * cosAlt);
    if (arg <= -1.0)
        AzRad = dms::PI;
    else if (arg >= 1.0)
        AzRad = 0.0;
    else
        AzRad = acos(arg);

    if (sinHA > 0.0)
        AzRad = 2.0 * dms::PI - AzRad; // resolve acos() ambiguity

    Alt.setRadians(AltRad);
    Az.setRadians(AzRad);
    std::clock_t stop = std::clock();
    cpuTime_EqToHz += double(stop - start) / double(CLOCKS_PER_SEC); // Accumulate time in seconds
    ++eqToHzCalls;

    // //Uncomment for XYZ version
    //  	double xr, yr, zr, xr1, zr1, sa, ca;
    // 	//Z-axis rotation by -LST
    // 	dms a = dms( -1.0*LST->Degrees() );
    // 	a.SinCos( sa, ca );
    // 	xr1 = m_X*ca + m_Y*sa;
    // 	yr  = -1.0*m_X*sa + m_Y*ca;
    // 	zr1 = m_Z;
    //
    // 	//Y-axis rotation by lat - 90.
    // 	a = dms( lat->Degrees() - 90.0 );
    // 	a.SinCos( sa, ca );
    // 	xr = xr1*ca - zr1*sa;
    // 	zr = xr1*sa + zr1*ca;
    //
    // 	//FIXME: eventually, we will work with XYZ directly
    // 	Alt.setRadians( asin( zr ) );
    // 	Az.setRadians( atan2( yr, xr ) );
}

void SkyPoint::HorizontalToEquatorial(const dms *LST, const dms *lat)
{
    double HARad, DecRad;
    double sinlat, coslat, sinAlt, cosAlt, sinAz, cosAz;
    double sinDec, cosDec;

    lat->SinCos(sinlat, coslat);
    alt().SinCos(sinAlt, cosAlt);
    Az.SinCos(sinAz, cosAz);

    sinDec = sinAlt * sinlat + cosAlt * coslat * cosAz;
    DecRad = asin(sinDec);
    cosDec = cos(DecRad);
    Dec.setRadians(DecRad);

    double x = (sinAlt - sinlat * sinDec) / (coslat * cosDec);

    //Under certain circumstances, x can be very slightly less than -1.0000, or slightly
    //greater than 1.0000, leading to a crash on acos(x).  However, the value isn't
    //*really* out of range; it's a kind of roundoff error.
    if (x < -1.0 && x > -1.000001)
        HARad = dms::PI;
    else if (x > 1.0 && x < 1.000001)
        HARad = 0.0;
    else if (x < -1.0)
    {
        //qWarning() << "Coordinate out of range.";
        HARad = dms::PI;
    }
    else if (x > 1.0)
    {
        //qWarning() << "Coordinate out of range.";
        HARad = 0.0;
    }
    else
        HARad = acos(x);

    if (sinAz > 0.0)
        HARad = 2.0 * dms::PI - HARad; // resolve acos() ambiguity

    RA.setRadians(LST->radians() - HARad);
    RA.reduceToRange(dms::ZERO_TO_2PI);
}

void SkyPoint::findEcliptic(const CachingDms *Obliquity, dms &EcLong, dms &EcLat)
{
    double sinRA, cosRA, sinOb, cosOb, sinDec, cosDec, tanDec;
    ra().SinCos(sinRA, cosRA);
    dec().SinCos(sinDec, cosDec);
    Obliquity->SinCos(sinOb, cosOb);

    tanDec          = sinDec / cosDec; // FIXME: -jbb div by zero?
    double y        = sinRA * cosOb + tanDec * sinOb;
    double ELongRad = atan2(y, cosRA);
    EcLong.setRadians(ELongRad);
    EcLong.reduceToRange(dms::ZERO_TO_2PI);
    EcLat.setRadians(asin(sinDec * cosOb - cosDec * sinOb * sinRA));
}

void SkyPoint::setFromEcliptic(const CachingDms *Obliquity, const dms &EcLong, const dms &EcLat)
{
    double sinLong, cosLong, sinLat, cosLat, sinObliq, cosObliq;
    EcLong.SinCos(sinLong, cosLong);
    EcLat.SinCos(sinLat, cosLat);
    Obliquity->SinCos(sinObliq, cosObliq);

    double sinDec = sinLat * cosObliq + cosLat * sinObliq * sinLong;

    double y = sinLong * cosObliq - (sinLat / cosLat) * sinObliq;
    //    double RARad =  atan2( y, cosLong );
    RA.setUsing_atan2(y, cosLong);
    RA.reduceToRange(dms::ZERO_TO_2PI);
    Dec.setUsing_asin(sinDec);
}


dms SkyPoint::angularDistanceTo(const SkyPoint *sp, double *const positionAngle) const
{
    // double dalpha = sp->ra().radians() - ra().radians() ;
    // double ddelta = sp->dec().radians() - dec().radians();
    CachingDms dalpha = sp->ra() - ra();
    CachingDms ddelta = sp->dec() - dec();

    // double sa = sin(dalpha/2.);
    // double sd = sin(ddelta/2.);

    // double hava = sa*sa;
    // double havd = sd*sd;

    // Compute the haversin directly:
    double hava = (1 - dalpha.cos()) / 2.;
    double havd = (1 - ddelta.cos()) / 2.;

    // Haversine law
    double aux = havd + (sp->dec().cos()) * dec().cos() * hava;

    dms angDist;
    angDist.setRadians(2. * fabs(asin(sqrt(aux))));

    if (positionAngle)
    {
        // Also compute the position angle of the line from this SkyPoint to sp
        //*positionAngle = acos( tan(-ddelta)/tan( angDist.radians() ) ); // FIXME: Might fail for large ddelta / zero angDist
        //if( -dalpha < 0 )
        //            *positionAngle = 2*M_PI - *positionAngle;
        *positionAngle =
            atan2f(dalpha.sin(), (dec().cos()) * tan(sp->dec().radians()) - (dec().sin()) * dalpha.cos()) * 180 / M_PI;
    }
    return angDist;
}