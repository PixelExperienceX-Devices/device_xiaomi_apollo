/*
   Copyright (c) 2015, The Linux Foundation. All rights reserved.
   Copyright (C) 2016 The CyanogenMod Project.
   Copyright (C) 2019-2020 The LineageOS Project.

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

#include <cstdlib>
#include <string.h>

#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>
#include <android-base/properties.h>

#include "property_service.h"
#include "vendor_init.h"

using android::base::GetProperty;
using std::string;

void property_override(char const prop[], char const value[], bool add = true)
{
    auto pi = (prop_info*) __system_property_find(prop);

    if (pi != nullptr)
        __system_property_update(pi, value, strlen(value));
    else if (add)
        __system_property_add(prop, strlen(prop), value, strlen(value));
}

void set_ro_build_prop(const string &source, const string &prop,
                       const string &value, bool product = false) {
    string prop_name;

    if (product)
        prop_name = "ro.product." + source + prop;
    else
        prop_name = "ro." + source + "build." + prop;

    property_override(prop_name.c_str(), value.c_str(), true);
}

void set_device_props(const string brand, const string marketname,
        const string device, const string model) {
    // list of partitions to override props
    string source_partitions[] = { "", "bootimage", "odm.", "product.",
                                   "system", "system_ext.", "vendor." };

    for (const string &source : source_partitions) {
        set_ro_build_prop(source, "brand", brand, true);
        set_ro_build_prop(source, "marketname", marketname, true);
        set_ro_build_prop(source, "device", device, true);
        set_ro_build_prop(source, "product", device, false);
        set_ro_build_prop(source, "model", model, true);
    }
}

void vendor_load_properties()
{
    /*
     * Detect device and configure properties
     */
    string hwc = GetProperty("ro.boot.hwc", "");
    string sku = GetProperty("ro.boot.product.hardware.sku", "");

    if (hwc == "CN") { // K30S Ultra (China)
            set_device_props("Xiaomi", "K30S Ultra", "apollo", "M2007J3SC");
    } else {
        if (sku == "pro") { // Mi 10T Pro
            set_device_props("Xiaomi", "Mi 10T Pro", "apollo", "M2007J3SG");
        } else { // Mi 10T
            set_device_props("Xiaomi", "Mi 10T", "apollo", "M2007J3SY");
        }
    }

    property_override("ro.build.fingerprint", "google/raven/raven:12/SQ1D.211205.016.A1/7957957:user/release-keys");
    property_override("ro.build.description", "apollo_global-user 11 RKQ1.200826.002 V12.5.5.0.RJDMIXM release-keys");
}
