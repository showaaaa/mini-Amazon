from django.urls import path
from . import views
from django.contrib.auth import views as auth_views

app_name = 'users_app'

urlpatterns = [
    # system login and sign up
    path('', views.index, name='index'),
    path('logout/', views.user_logout, name='logout'),
    path('register/', views.register, name='register'),
    path('user_login/', views.user_login, name='login'),
    # home page
    path('home/', views.home, name='home'),
    path('in_cart/<int:item_id>/',views.in_cart, name = 'in_cart'),
    path('place_order',views.place_order, name = 'place_order'),
    # # orderStatus
    path('remove_item/<int:item_id>/', views.remove_item, name='remove_item'),
    path('trace_order', views.trace_order, name='trace_order'),
    path('cataLog/', views.cataLog, name='cataLog'),

    path('success', views.success, name='success'),
    path('item_details/<int:itemid>/', views.post_commennt, name='item_details'),   
]