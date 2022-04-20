# webplayer

- 网页播放器，无插件，canvas标签，支持h265,h264格式,rtsp实时流播放。
- 暂时未实现音频

### 已实现的API
- 播放
```
//myCanvas : canvas标签的id
//width :canvas标签的宽度
//height :canvas标签的高度
var playerController = new PlayerController("myCanvas", width, height);

//webURL ：websocket url
//rtspURL : rtsp url
//class : PlayerController
playerController.PlayRtsp(webURL, rtspURL);

```
- 停止

```
playerController.stop();
```

- 暂停
```
playerController.pause();//暂停
playerController.resume();// 播放
playerController.togglePause();// 暂停/播放
```
- 倍速

```
//speed :速度 
//speed范围：[0.5, 4]
playerController.setScale(speed);
```
- 设置画布大小
```
playerController.setCanvasSize(width, height);
```