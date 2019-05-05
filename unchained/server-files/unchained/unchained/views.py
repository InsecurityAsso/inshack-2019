from django.views.generic.base import TemplateView
from django.views.decorators.http import require_http_methods
from django.http import HttpResponse
from django.shortcuts import redirect
from django.contrib import messages
from django.core import signing

CRAZY = "crazy guy who wants to be admin"


class Home(TemplateView):
    template_name = "unchained/home.html"

    def get_context_data(self, **kwargs):
        context = super(Home, self).get_context_data(**kwargs)
        user = self.request.COOKIES.get('username', signing.dumps('anonymous'))
        try:
            user = signing.loads(user)
        except Exception:
            user = CRAZY
        context["user"] = user
        context["admin"] = user == "admin"
        context["flag"] = open("/tmp/flag").read()
        return context


@require_http_methods(["POST"])
def login(r):
    username = r.POST.get("username")
    if username == "admin":
        messages.error(r, "Sorry, but I AM the ONLY admin, go play with old boring userland")
        username = CRAZY
    
    response = redirect('home')
    response.set_cookie('username', signing.dumps(username))
    return response

