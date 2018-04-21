function print(...)
		component.invoke(component.list("aurum_debug")(), "printDebug", ...)
return ...
end

print("### Filesystem testing ###")

local fs = component.list("filesystem")()

h = print(component.invoke(fs, "open", "fil"))

print(component.invoke(fs, "read", h))
print(component.invoke(fs, "read", h))

print(component.invoke(fs, "close", h))

print("### Passed ###")


