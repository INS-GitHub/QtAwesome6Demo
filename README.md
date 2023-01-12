# Get Icon by FontAwesome with Qt
**Base of https://github.com/gamecreature/QtAwesome**


## New 
- Update FontVision to 6.1.1 pro
- Add brands icons
- Add search tool
  
## Use
1. add "include(QtAwesome6/QtAwesome.pri)" in *.pro file
2. add "QAwesome->initFontAwesome();" before use   
   Can't init by SingletonLazzy (QFontDatabase error) 

``` 
QIcon icon = QAwesome->icon(enum, m_color); 
```

``` C++
QVariantMap options;
options.insert("color" , QColor(255, 0 ,0));
QPushButton* musicButton = new QPushButton(awesome->icon(fa::fa_solid, fa::music, options), "Music");  
```

## Others
https://github.com/gamecreature/QtAwesome#readme
  
