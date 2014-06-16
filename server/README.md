###Protocol
- Send UTF-8 encoded JSON to straw.imyoyo.net:8282 (UDP)
- JSON example: 
```javascript
{
   "cpuUsage":{
      "total":0.37,
      "unit":[
         0.3,
         0.4,
         0.6,
         0.7
      ]
   },
   "memoryUsage":{
      "max":1048576,
      "current":102400
   },
   "machineId":"java_agent_test",
   "groupKey":"ce3bd840-f0a7-11e3-ac10-0800200c9a66"
}
```
- Java agent example: https://github.com/stjeong/Straw/blob/master/server/app/receiver/UTF8StringSendingUdpClient.java
