# qtutils

This is a simple utility library for the Qt framework.
It allows to connnect and disconnect signals and slots easily, especially when a lot of slots from one Qt object should be connected to signals of another Qt object.

Now instead of writing some boring code like
```
connect(emitterObj, SIGNAL(onSignal1()), handlerObj, SLOT(handleSignal1()));
connect(emitterObj, SIGNAL(onSignal2()), handlerObj, SLOT(handleSignal2()));
...
```

you can write following

```
QtUtils::makeConnector(emitterObj, handlerObj)
  .connect(SIGNAL(onSignal1()), SLOT(handleSignal1()))
  .connect(SIGNAL(onSignal2()), SLOT(handleSignal2()));
```

or
```
QtUtils::makeConnector(emitterObj, handlerObj)
  .connect(&Emitter::onSignal1, &Handler::handleSignal1)
  .disconnect(&Emitter::onSignal1, &Handler::handleSignal1);
```

The library is header only ATM, but you can build and run unit tests:
```
(cd qtutils && qmake && make && ./unittests)
```
