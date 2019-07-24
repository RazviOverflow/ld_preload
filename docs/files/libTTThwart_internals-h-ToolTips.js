NDContentPage.OnToolTipsLoaded({38:"<div class=\"NDToolTip TFunction LC\"><div id=\"NDPrototype38\" class=\"NDPrototype WideForm CStyle\"><table><tr><td class=\"PBeforeParameters\"><span class=\"SHKeyword\">void</span> check_parameters_properties(</td><td class=\"PParametersParentCell\"><table class=\"PParameters\"><tr><td class=\"PModifierQualifier first\"><span class=\"SHKeyword\">const</span>&nbsp;</td><td class=\"PType\"><span class=\"SHKeyword\">char</span>&nbsp;</td><td class=\"PNamePrefix\">*</td><td class=\"PName last\">path,</td></tr><tr><td class=\"PModifierQualifier first\"><span class=\"SHKeyword\">const</span>&nbsp;</td><td class=\"PType\"><span class=\"SHKeyword\">char</span>&nbsp;</td><td class=\"PNamePrefix\">*</td><td class=\"PName last\">caller_function_name</td></tr></table></td><td class=\"PAfterParameters\">)</td></tr></table></div><div class=\"TTSummary\">Checks properties of the given parameters, this is, the given path and inode. Checking properties in this context means checking if a file_object_info with the same path already exists in the array. If it doesn\'t, insert it; othwerwise (if it does) compare the given inode and the inode of the file_object_info as well as the file mode and the device id. If they\'re equal continue execution; otherwise abort execution because a possible TOCTTOU is detected.</div></div>",42:"<div class=\"NDToolTip TFunction LC\"><div id=\"NDPrototype42\" class=\"NDPrototype WideForm CStyle\"><table><tr><td class=\"PBeforeParameters\"><span class=\"SHKeyword\">void</span> get_fs_and_initialize_checking_functions(</td><td class=\"PParametersParentCell\"><table class=\"PParameters\"><tr><td class=\"PModifierQualifier first\"><span class=\"SHKeyword\">const</span>&nbsp;</td><td class=\"PType\"><span class=\"SHKeyword\">char</span>&nbsp;</td><td class=\"PNamePrefix\">*</td><td class=\"PName last\">path</td></tr></table></td><td class=\"PAfterParameters\">)</td></tr></table></div><div class=\"TTSummary\">Determines what the file system of a given path is and, based on that, initialize check_parameters_properties function accordingly. Please refer to that function in order to know what different initializations means.</div></div>",58:"<div class=\"NDToolTip TType LC\"><div class=\"TTSummary\">Represents specific metadata of each file used by any of the hooked functions.</div></div>"});