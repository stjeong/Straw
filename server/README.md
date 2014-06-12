###Protocol
- Send UTF-8 encoded JSON to straw.imyoyo.net:8282 (UDP)
- JSON example: 
{"cpuUsage":{"total": 0.37}, "machineId":"java_agent_test", "groupKey":"ce3bd840-f0a7-11e3-ac10-0800200c9a66"}
- Java agent example: https://github.com/stjeong/Straw/blob/master/server/app/receiver/UTF8StringSendingUdpClient.java
