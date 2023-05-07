from django.db import models
# Create your models here.
from django.utils import timezone
from django.utils.timezone import now
from django.urls.base import reverse
from datetime import datetime 
# Create your models here.
class Warehouse(models.Model):
    loc_x = models.IntegerField(default=-1, blank=True)
    loc_y = models.IntegerField(default=-1, blank=True)
    def __str__(self):
        return f'({self.loc_x}, {self.loc_y})'

class Category(models.TextChoices):
    ELECTRONIC ='electronic'
    FOOD = 'food'
    DRINK ='drink'
    DAILY_SUPPLY = 'daily_supply'
    OTHERS = 'others'
    
class Item(models.Model):
    # item_id = models.IntegerField(primary_key=True)
    name = models.CharField(max_length=100)
    description = models.CharField(max_length=100, null=False, default='None')
    price = models.FloatField(default=9.9, null=False, blank=False)
    cataLog = models.CharField(
        max_length=50, choices=Category.choices, default= Category.OTHERS)
    class Meta:
        db_table = 'item'
        
class Shopping_item(models.Model):
    # item_id = models.IntegerField(primary_key=True)
    name = models.CharField(max_length=100)
    price = models.FloatField(default=9.9, null=False, blank=False)
    quantity = models.IntegerField(default=1, null=False)
    customer_name = models.CharField(max_length=50)
    class Meta:
        db_table = 'shopping_item'

class OrderStatusList(models.TextChoices):
    PACKING ='packing'
    PACKED = 'packed'
    LOADING ='loading'
    LOADED = 'loaded'
    DELIVERING = 'delivering'
    DELIVERED = 'delivered'

class Order(models.Model):
    pack_id = models.AutoField(primary_key=True)
    customer_name = models.CharField(max_length=50)
    addr_x = models.IntegerField()
    addr_y = models.IntegerField()
    time = models.TimeField(auto_now=True)
    status = models.CharField(
        max_length=50, choices=OrderStatusList.choices, default=OrderStatusList.PACKING)
    amount = models.IntegerField()
    ups_id = models.IntegerField(default=1)
    item = models.ForeignKey(Item, on_delete=models.SET_NULL, null=True)
    itemid = models.IntegerField(default=-1)
    truck_id = models.IntegerField(default=-1)
    
    wh_id = models.IntegerField(default=1)
    price = models.FloatField(max_length=1000)
    truck_id = models.IntegerField(default=-1)
    version = models.IntegerField(default=0)  # need default value.
    class Meta:
        db_table = 'orders'


class Inventory(models.Model):
    item = models.ForeignKey(Item, on_delete=models.SET_NULL, null=True)
    item_amount = models.IntegerField()
    wh_id = models.IntegerField()
    version = models.IntegerField(default=0)  # need default value.

    class Meta:
        unique_together = ["item", "wh_id"]
        db_table = 'inventory'

# 启动时创建
class UserProfile(models.Model):
    userName = models.CharField(max_length=50, primary_key=True)
    addrX = models.CharField(max_length=10, null=True)
    addrY = models.CharField(max_length=10, null=True)
    upsID = models.CharField(max_length=10, null=True)

    def get_absolute_url(self):
        return reverse('home')


class Comment(models.Model):
    item_id = models.IntegerField(null=False)
    content = models.TextField()
    user_name = models.CharField(max_length=50, null=False)
    create_time = models.TimeField(auto_now=True)
    class Meta:
        db_table = 'comment'
