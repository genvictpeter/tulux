/*
 *  Copyright (c) 2020, The Linux Foundation. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *    * Neither the name of The Linux Foundation nor the names of its
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 *  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 *  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 *  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 *  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 *  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

Steps to setup Socks Proxy:
1. Run vlan_sample_app/telsdk_console_app to create vlan.
2. Update VLAN mode at IPA
   echo vlan:eth > /etc/data/ipa_config.txt
3. Modify /etc/data/ipa/IPACM_cfg.xml to the following:
   <IPACMMPDN>
      <IPACMMPDNEnabled>0</IPACMMPDNEnabled> -> This should be set to 0.
   </IPACMMPDN>
4. From shell, open file " /etc/data/l2tp_cfg.xml" for editing and set DeviceModeCfg to 2
   <DeviceModeCfg>2</DeviceModeCfg>
5. From shell, open file "/etc/data/qti_socksv5_auth.xml" for editing and set authentication method.
    <SOCKSV5_AUTH_METHOD_CFG>
            <SOCKSV5_AUTH_METHOD>USERNAME_PASSWORD</SOCKSV5_AUTH_METHOD>
    </SOCKSV5_AUTH_METHOD_CFG>
6. From shell, open file "/etc/data/qti_socksv5_conf.xml" to set LAN configuration.
   Lan configuration is <vlan if>0.<vlan id>
   For instance, if vlan in step 1 was created on ETH interface with Vlan id 8, LAN config will be
    <LAN_CFG>
            <LAN_IFACE>eth0.8</LAN_IFACE>
    </LAN_CFG>
7. Reboot
