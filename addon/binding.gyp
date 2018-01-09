{
    "targets": [
        {
            "target_name": "node-plugin",
            "sources": [ "addon.cpp","plugin.h","plugin.c" ],
			"include_dirs" : ["<!(node -e \"require('nan')\")"]
        }
    ]
}