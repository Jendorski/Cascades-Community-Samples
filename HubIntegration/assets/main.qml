import bb.cascades 1.2

NavigationPane {
    id: navigationPane
    
	Page {
	    Container {
	        layout: StackLayout {
	            
	        }
	        Container {
	            layout: StackLayout {
	                orientation: LayoutOrientation.LeftToRight
	            }
	            Label {
	                // Localized text with the dynamic translation and locale updates support
	                text: qsTr("Hello Hub Integration") + Retranslate.onLocaleOrLanguageChanged
	                textStyle.base: SystemDefaults.TextStyles.BigText
	            }
	        }
	    }
	}
	
	onPopTransitionEnded: {
        _app.closeCard();
    }
}
