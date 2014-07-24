COMPILER = g++
CFLAGS += -g -O0 -lpthread 
manager:
#	$(COMPILER) $(CFLAGS) -o manager  manager.cpp CRuleManager.cpp CRuleManager.h common.cpp
	g++ -o manager manager.cpp CRuleManager.cpp CRuleManager.h CLogger.h CLogger.cpp CDbManager.h CDbManager.cpp common.cpp -lpthread -g
clean:
	rm -f *.o *.gch manager *.out
