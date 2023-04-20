/*
   Copyright (c) 2015, The Linux Foundation. All rights reserved.
   Copyright (C) 2016 The CyanogenMod Project.
   Copyright (C) 2019 The LineageOS Project.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.
   THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
   ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <fstream>
#include <string.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

#include <android-base/properties.h>
#include "property_service.h"
#include "vendor_init.h"

using android::base::GetProperty;
using android::base::SetProperty;
using std::string;

void property_override(string prop, string value)
{
    auto pi = (prop_info *)__system_property_find(prop.c_str());

    if (pi != nullptr)
        __system_property_update(pi, value.c_str(), value.size());
    else
        __system_property_add(prop.c_str(), prop.size(), value.c_str(), value.size());
}

void load_dalvik_properties() {
    struct sysinfo sys;

    sysinfo(&sys);
    if (sys.totalram < 6144ull * 1024 * 1024) {
        // from - phone-xhdpi-6144-dalvik-heap.mk
        property_override("dalvik.vm.heapstartsize", "16m");
        property_override("dalvik.vm.heapgrowthlimit", "256m");
        property_override("dalvik.vm.heapsize", "512m");
        property_override("dalvik.vm.heapmaxfree", "32m");
    } else {
        // 8GB & 12GB RAM
        property_override("dalvik.vm.heapstartsize", "32m");
        property_override("dalvik.vm.heapgrowthlimit", "512m");
        property_override("dalvik.vm.heapsize", "768m");
        property_override("dalvik.vm.heapmaxfree", "64m");
    }

    property_override("dalvik.vm.heaptargetutilization", "0.5");
    property_override("dalvik.vm.heapminfree", "8m");
}

void vendor_load_properties()
{
    load_dalvik_properties();

    string brand = "Redmi";
    string model;
    string marketname;

    string region = GetProperty("ro.boot.hwc", "");
    string board = GetProperty("ro.boot.product.hardware.sku", "");
    string name = board;

    property_override("ro.product.board", board);
    property_override("ro.product.device", board);
    property_override("ro.product.vendor.device", board);

    property_override("ro.oem_unlock_supported", "0");

    if (board == "chopin") {
        if (region == "CN"){
            model = "M2104K10AC";
            marketname = "Redmi Note 10 Pro";
        } else {
            brand = "POCO";
            model = "21061110AG";
            marketname = "POCO X3 GT";
            name = "chopin_global";
        }
    } else if (board == "choping") {
        brand = "POCO";
        model = "21061110AG";
        marketname = "POCO X3 GT";
        name = "chopin_global";
    } else if (board == "chopinin") {
        model = "21041110AI";
        marketname = "Redmi Note 10 Pro";
    }

    // Override all partitions' props
    string prop_partitions[] = {"", "odm.", "vendor."};
    for (const string &prop : prop_partitions) {
        property_override(string("ro.product.") + prop + string("brand"), brand);
        property_override(string("ro.product.") + prop + string("model"), model);
        property_override(string("ro.product.") + prop + string("marketname"), marketname);
        property_override(string("ro.product.") + prop + string("name"), name);
    }
}