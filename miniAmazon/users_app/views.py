from django.shortcuts import render, redirect
# from amazon.utils import *
from django.contrib import messages
from .forms import UserRegisterForm, UpdateProfileForm, PurchaseForm, QuantityForm, CommentForm
from django.contrib.auth import authenticate, login, logout, update_session_auth_hash
from django.contrib.auth.decorators import login_required
from .models import Item, UserProfile, Order, OrderStatusList, Shopping_item
from .helper import *
def index(request):

    return redirect('users_app:home')

def register(request):
    registered = False
    if request.method == 'POST':
        form = UserRegisterForm(request.POST)
        if form.is_valid():
            form.save()
            registered = True
            return redirect('users_app:login')
        else:
            print(form.errors)
    else:
        form = UserRegisterForm()
    return render(request, 'register.html', {'user_form': form,'registered':
        registered})


def user_login(request):
    if request.method == 'POST':
        username = request.POST.get('username')
        password = request.POST.get('password')
        user = authenticate(username=username, password=password)
        if user:
            if user.is_active:
                login(request, user)
                return redirect('users_app:cataLog')
        else:
            msg = "Invalid username or password. Please try again."
            return render(request, 'login.html', {'msg': msg})
    else:
        return render(request, 'login.html', {})

@login_required
def user_logout(request):
    logout(request)
    return redirect('users_app:home')


# @login_required
def home(request):
    # Item.objects.all().delete()
    # create profile in database for every new user
    if request.user.is_authenticated:
        ls = UserProfile.objects.filter(userName=request.user.username)
        if ls.exists() is not True:
            profile = UserProfile.objects.create(userName=request.user.username)

            profile.save()

    return render(request, 'home.html')

item_names = ["iPad", "iPhone", "Macbook", "iPod", "Shampoo", "Pepsi Cola", "Lobster", "Gift card"]
prices = [499, 999, 1199, 199, 19.99, 9.99, 39.99, 9.99]
description = ["iPad", "iPhone", "Macbook", "iPod", "Shampoo", "Pepsi Cola", "Lobster", "Gift card"]
cats = ['electronic', 'electronic', 'electronic', 'electronic', 'daily_supply', 'drink', 'food', 'others']

@login_required
def cataLog(request):
    if request.method == 'GET':     
        items = Item.objects.all()
        if items.exists() is not True:
            for i in range(len(item_names)):
                item = Item.objects.create()
                item.name = item_names[i]
                item.price = prices[i]
                item.cataLog = cats[i]
                item.description = description[i]
                item.save()
        
        electronic_items = Item.objects.filter(cataLog='electronic')
        food_items = Item.objects.filter(cataLog='food')
        drink_items = Item.objects.filter(cataLog='drink')
        daily_supply_items = Item.objects.filter(cataLog='daily_supply')
        print(daily_supply_items)
        other_items = Item.objects.filter(cataLog='others')
        
    return render(request, 'item.html', {'electronic_items': electronic_items, 'food_items': food_items,\
        'drink_items': drink_items, 'daily_supply_items': daily_supply_items, 'other_items': other_items})

@login_required
def in_cart(request, item_id):
    if request.method =='POST':
        form = QuantityForm(request.POST)
        
        if form.is_valid():
            product_num = form.cleaned_data["productNum"]
            # whID = get_closest_wh(address_x, address_y)
            # wh = Warehouse.objects.filter(id=whID).first()
            product = Item.objects.filter(id = item_id).first()
            existed_shoppint_item = Shopping_item.objects.filter(name=product.name,\
                customer_name=request.user.username)
            if existed_shoppint_item.exists():
                existed_shoppint_item = existed_shoppint_item.first()
                existed_shoppint_item.quantity = existed_shoppint_item.quantity+product_num
                existed_shoppint_item.save()
            else:
                new_shopping_item = Shopping_item.objects.create(name=product.name, \
                    price=product.price, quantity=product_num, customer_name=request.user.username)
                new_shopping_item.save()
            return redirect('users_app:success')
        
        else:
            return HttpResponse(form.errors)
    else:
        form = QuantityForm()
    return render(request,'add_item.html', {'form':form})

@login_required
def place_order(request):
    if request.method =='POST':
        form = PurchaseForm(request.POST)
        if form.is_valid():
            addr_x = form.cleaned_data["address_x"]
            addr_y = form.cleaned_data["address_y"]
            ups_id = form.cleaned_data["ups_id"]
            # whID = get_closest_wh(address_x, address_y)
            # wh = Warehouse.objects.filter(id=whID).first()
            orders = []
            shopping_items = Shopping_item.objects.filter(customer_name=request.user.username)
            for shop_item in shopping_items:
                item = Item.objects.filter(name=shop_item.name).first()
                order = Order.objects.create(addr_x=addr_x, addr_y=addr_y, amount=shop_item.quantity, ups_id=ups_id, \
                    item=item, itemid=item.id, price=shop_item.quantity*shop_item.price, customer_name=request.user.username)
                order.save()
                orders.append(order)
            #TODO: send order to server
            sendToserver(orders)
            for shop_item in shopping_items:
                shop_item.delete()
            return redirect('users_app:success')
        else:
            return HttpResponse(form.errors)
    else:
        form = PurchaseForm()
        shopping_items = Shopping_item.objects.filter(customer_name=request.user.username)
    return render(request,'place_order.html', {'form':form, "items": shopping_items})

@login_required
def trace_order(request):
    if request.method == 'GET':
        
        user_orders = Order.objects.filter(customer_name=request.user.username)
        
    return render(request, 'trace_orders.html', {'orders': user_orders})

@login_required
def success(request):
    return render(request, 'place_success.html')

@login_required
def remove_item(request, item_id):
    item = Shopping_item.objects.filter(id=item_id).first()
    item.delete()
    return redirect('users_app:place_order')

@login_required
def post_commennt(request, itemid):
    user_name = request.user.username
    comments = Comment.objects.filter(item_id=itemid)
    description = Item.objects.filter(id=itemid).first().description
    palced_order = Order.objects.filter(customer_name=request.user.username, itemid=itemid)
    can_comment = True
    if palced_order.exists() is not True:
        can_comment = False
    if request.method == 'POST':
        comment_form = CommentForm(request.POST)
        if comment_form.is_valid():
            content = comment_form.cleaned_data["content"]
            comment = Comment.objects.create(content=content,\
                user_name=user_name, item_id=itemid)
            comment.save()
            return redirect('users_app:item_details', itemid=itemid)
        else:
            return HttpResponse("Please resubmit the comment")
        # 处理错误请求
    else:
        comment_form = CommentForm(request.POST)
    
    return render(request,'item_details.html', {'form':comment_form, 'item_comments': comments,\
        'item_name': description, 'can_comment': can_comment})

    