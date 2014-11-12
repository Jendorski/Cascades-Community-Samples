import bb.cascades 1.2

Page {
    id: page
    objectName: "page"
    
    titleBar: 
    TitleBar {
        title: qsTr("Compose Item (not implemented")
    }
        
    Container {
        id: container
        objectName: "container"
        
        layout: StackLayout {
            
        }
        Container {
            id: fromContainer
            objectName: "fromContainer"
            
            leftPadding: 10
            rightPadding: 10
            topPadding: 10
            bottomPadding: 10
            
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            
            Container {
                id: fromLabelContainer
                objectName: "fromLabelContainer"
                
                leftPadding: 10
                rightPadding: 10
                topPadding: 10
                bottomPadding: 10
                
                layout: DockLayout {
                }
                
	            Label {
	                id: fromLabel
	                objectName: "fromLabel"
	                
                    leftMargin: 5
                    rightMargin: 5
                    
                    horizontalAlignment: HorizontalAlignment.Right
                    verticalAlignment: verticalAlignment.Center
                    
	                // Localized text with the dynamic translation and locale updates support
	                text: qsTr("From: ") + Retranslate.onLocaleOrLanguageChanged
	                textStyle.base: SystemDefaults.TextStyles.NormalText
	                textStyle.fontWeight: FontWeight.Bold
	            }
	        }
            Label {
                id: fromName
                objectName: "fromName"
                
                // Localized text with the dynamic translation and locale updates support
                text: qsTr("") + Retranslate.onLocaleOrLanguageChanged
                textStyle.base: SystemDefaults.TextStyles.NormalText
            }
        }
        Container {
            id: subjectContainer
            objectName: "subjectContainer"
            
            leftPadding: 10
            rightPadding: 10
            topPadding: 10
            bottomPadding: 10
            
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            
            Container {
                id: subjectLabelContainer
                objectName: "subjectLabelContainer"
                
                leftPadding: 10
                rightPadding: 10
                topPadding: 10
                bottomPadding: 10
                
                layout: DockLayout {
                }
                
	           Label {
	                id: subjectLabel
	                objectName: "subjectLabel"
	                                                
	                leftMargin: 5
	                rightMargin: 5
	                
                    horizontalAlignment: HorizontalAlignment.Right
	                verticalAlignment: verticalAlignment.Center
	                
	                // Localized text with the dynamic translation and locale updates support
	                text: qsTr("Subject: ") + Retranslate.onLocaleOrLanguageChanged
	                textStyle.base: SystemDefaults.TextStyles.NormalText
	                textStyle.fontWeight: FontWeight.Bold
	            }
	        }
        	TextArea {
                id: subject
                objectName: "subject"
                enabled: false
                
                // Localized text with the dynamic translation and locale updates support
                text: qsTr("") + Retranslate.onLocaleOrLanguageChanged
                textStyle.base: SystemDefaults.TextStyles.NormalText
            }
        }
        Container {
            id: bodyContainer
            objectName: "bodyContainer"
            
            leftPadding: 10
            rightPadding: 10
            topPadding: 10
            bottomPadding: 10
            
            preferredHeight: 400
            
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            
            TextArea {
                id: body
                objectName: "body"
                enabled: false
                
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                
                // Localized text with the dynamic translation and locale updates support
                text: qsTr("") + Retranslate.onLocaleOrLanguageChanged
                textStyle.base: SystemDefaults.TextStyles.NormalText
            }
        }
    }
    
    
    actions: [
        /*
        ActionItem {
            title: "Back"
            ActionBar.placement: ActionBarPlacement.OnBar
            imageSource: "asset:///actions/ActionBar_Back.png"
            onTriggered: {
            }
        } */
    ]

}
