NDContentPage.OnToolTipsLoaded({7:"<div class=\"NDToolTip TFunction LC\"><div id=\"NDPrototype7\" class=\"NDPrototype WideForm CStyle\"><table><tr><td class=\"PBeforeParameters\"><span class=\"SHKeyword\">extern int</span> open_wrapper(</td><td class=\"PParametersParentCell\"><table class=\"PParameters\"><tr><td class=\"PModifierQualifier first\"><span class=\"SHKeyword\">const</span>&nbsp;</td><td class=\"PType\"><span class=\"SHKeyword\">char</span>&nbsp;</td><td class=\"PNamePrefix\">*</td><td class=\"PName last\">path,</td></tr><tr><td class=\"first\"></td><td class=\"PType\"><span class=\"SHKeyword\">int</span>&nbsp;</td><td></td><td class=\"PName last\">flags,</td></tr><tr><td class=\"first\"></td><td class=\"PType\">va_list&nbsp;</td><td></td><td class=\"PName last\">variable_arguments</td></tr></table></td><td class=\"PAfterParameters\">)</td></tr></table></div><div class=\"TTSummary\">The open wrapper guarantees, insures original_open is initialized.&nbsp; It\'s used by other inner functions in order to avoid open() recursivity and overhead. In adittion, it deals with ellipsis (variable arguments) since open is a variadic function.</div></div>"});