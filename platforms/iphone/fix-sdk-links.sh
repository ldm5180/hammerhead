#!/bin/bash

# Fix IOKit symlinks
for vers in '2.0' '2.1' '2.2' '2.2.1' '3.0' '3.1'; do 
	FRAMEWORK_OS=/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS${vers}.sdk/System/Library/Frameworks; 
	FRAMEWORK_SIM=/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator${vers}.sdk/System/Library/Frameworks; 
	IOKIT_FRAMEWORK_OS=${FRAMEWORK_OS}/IOKit.framework; 
	IOKIT_FRAMEWORK_SIM=${FRAMEWORK_SIM}/IOKit.framework; 
	
	(cd ${IOKIT_FRAMEWORK_OS}/Versions && sudo ln -sf A Current); 
	(cd ${IOKIT_FRAMEWORK_OS}/Versions/A && sudo ln -sf ${IOKIT_FRAMEWORK_SIM}/Versions/A/Headers Headers); 
	(cd ${IOKIT_FRAMEWORK_OS} && sudo ln -sf Versions/Current/IOKit IOKit); 
	(cd ${IOKIT_FRAMEWORK_OS} && sudo ln -sf Versions/Current/Headers Headers); 
done; 

# Disable Carbon framework in Simulator
for vers in '3.0'; do 
	FRAMEWORK_OS=/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS${vers}.sdk/System/Library/Frameworks; 
	FRAMEWORK_SIM=/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator${vers}.sdk/System/Library/Frameworks; 
        if [ -d ${FRAMEWORK_SIM}/Carbon.framework ]; then
            (cd ${FRAMEWORK_SIM} && sudo mv Carbon.framework Carbon.framework.disabled); 
        fi
done

