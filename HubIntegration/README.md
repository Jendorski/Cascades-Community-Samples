HubIntegration is a sample app that demonstrates best practices for developers working on apps that integrate with the Hub using push.

This sample app represents the foreground component that includes a card to view Hub items created by the background component, 
HeadlessHubIntegration.  When you build this sample app after building HeadlessHubIntegration, it will incorporate the executable 
from that project with this one to make a single BAR package for installation. 

Prerequisites:

Developers cannot install or run this application without first getting their signing accounts updated for Hub Integration.  
To do so, developers must apply for Hub integration for their app via the online form here:

https://developer.blackberry.com/jam/headless/

Note that if you want Hub integration without push, you also need long running headless.  Developer do not need to apply for this separately 
but  should specify the sync model as pull when applying.  Upon approval, they will be contacted to get information required for 
updating your signing accounts.

To add Hub items when testing this sample app before integrating with your app, download and import the SimplePushServer sample 
from the Cascades-Community-Samples repo on GitHub. More on how this works in the sections below:

Notes:

How to build this sample app:

- import SimplePushServer, HeadlessHubIntegration, and HubIntegration projects into your workspace via the Momentics IDE
- update PushManager.cpp in the HeadlessHubIntegration project with your app's or testing push credentials and URL
- build the HeadlessHubIntegration project
- update the SimplePushServer sample's SimplePushServer.cpp with your app's or testing push credentials and URL
- build the SimplePushServer project and deploy this to your device
- build the HubIntegration project
- deploy the app to your device and close the foreground app after it starts up
- note that no Hub account is setup at this time

How to run this sample app:

- the headless component of the app should be running as a service
- launch the SimplePushServer sample
- query the pins and confirm that your device's pin is in the list
- send a low priority or medium priority message
- a Hub item should be added with Test as the source, the title of your message should appear just below as the subject.  
  You will not hear any sound, feel a vibration or see an LED flash at this priority but you will see a splat on your 
  Hub account and the Hub sidebar item.
- if you click on the Hub item, you should be able to launch the card and see the source, title and body of the message 
  you sent.  Click back to return to the Hub.
- go back to SimplePushServer and send a high priority message
- this time, you should see the Hub entry appear first as an instant preview.  If you click on the instant preview, it 
  will go to the Hub and launch the card.  The item also shows up in the Hub.  Unlike the previous test, you will hear 
  a notification, feel a vibration or see the LED flash, depending on your notification settings.

Please note that if you have a mechansim for pushing messages you can use that instead of SimplePushServer.  Just make 
sure the message you send is formatted the same way that the SimplePushServer app sends it.

Although this sample is push based, it is relatively simple to convert it a sync based sample.  The simplest way is as follows:

- add _sys_headless_nostop permission to the bar descriptor
- add a QTimer to the HeadlessHubIntegration class that fires once every five minutes
- in the timeout handler, add code to check your server and download new messages
- call processNewMessage() or an equivalent function that operates in a similar way to add the item to the Hub
- remove the push invocation handling code and push targets from the bar descriptor of both projects

Additional customizations when integrating with your own app:

HeadlessHubIntegration:

- TestAccount - Review this class, derived from HubAccount, and substitute it with your own version of this class 
                customized for your app's account
- HubAccount - contains various methods for managing a Hub account and its items (most developers do not need to change this)
- HeadlessHubIntegration - modify UDSUtil initialization parameters to ensure your service name is unique and the 
                           proper hub assets folder name in the UI app is provided
- UDSUtil - a Qt wrapper around the native UDS library to simplify UDS operations
- HubCache - a class that implements a simple Hub cache using QSettings (feel free to substitute this with your own
             class to implement a cache via another mechanism such as SQLLite database caching if you need better performance)
- PushManager - a utility class for processing push data

Hub Integration:
- look at this sample for guidance on what to add to your app to add the cards needed for proper Hub integration
- bar descriptor 
	- modify the invocation targets to match the names defined in TestAccount or your equivalent class
	- modify the hub item MIME type in the invocation target definitions to match the MIME type defined in TestAccount 
	  or your equivalent class
	- keep in mind that the headless app launches first to prompt for permissions.  Please change its name so that it looks like it
	  belongs to your app to avoid user confusion

For more information about our other Open Source projects, visit:

BlackBerry Open Source microsite (http://blackberry.github.com)

To check the Samples Catalog, visit:
Samples Catalog (http://blackberry.github.com/samples)

For more information about Cascades development, visit:
The Cascades microsite (http://developers.blackberry.com/cascades)

