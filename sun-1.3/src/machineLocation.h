/* Copyright 2017 Nicholas C. Strauss (strauss@positive-internet.com)
   This program is distributed under the terms of the GNU General Public License.
   See File COPYING for full details. 
*/
#ifndef __STARTIME_MACHINELOCATION__
#define __STARTIME_MACHINELOCATION__
class machineLocation
{
 public:
  machineLocation(double lat, double lon);
  virtual ~machineLocation();
  double _latitude;
  double _longitude;
};
#endif
