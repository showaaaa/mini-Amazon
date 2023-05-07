from django import forms
from django.contrib.auth.models import User
from django.contrib.auth.forms import UserCreationForm
from .models import UserProfile, Comment


class UserRegisterForm(UserCreationForm):
    username = forms.CharField(widget=forms.TextInput(attrs={"placeholder": "Username",
                                                             "class": "form-control"}))
    email = forms.EmailField(widget=forms.EmailInput(attrs={"placeholder": "Email",
                                                            "class": "form-control"}))
    password1 = forms.CharField(widget=forms.PasswordInput(attrs={"placeholder": "Password",
                                                                  "class": "form-control"}))
    password2 = forms.CharField(widget=forms.PasswordInput(attrs={"placeholder": "Password check",
                                                                  "class": "form-control"}))

    class Meta:
        model = User
        fields = ('username', 'email', 'password1', 'password2')




class UserUpdateForm(forms.ModelForm):
    email = forms.EmailField()

    class Meta:
        model = User
        fields = ['username', 'email']

class UpdateProfileForm(forms.ModelForm):
    addrX = forms.CharField(max_length=10)   
    addrY = forms.CharField(max_length=10) 
    upsID = forms.CharField(max_length=10, required=False) 

    class Meta:
        model = UserProfile
        fields = ['addrX','addrY','upsID']
        
class PurchaseForm(forms.Form):
    
    address_x = forms.IntegerField(label='Address x',required=True)
    address_y = forms.IntegerField(label='Address y',required=True)
    ups_id = forms.IntegerField(label='UPS ID',required=False) 
    
class QuantityForm(forms.Form):
    productNum = forms.IntegerField(label='Number of products',required=True)

class CommentForm(forms.ModelForm):
    class Meta:
        model = Comment
        fields = ['content']
    
# class addShoppingCartForm(forms.ModelForm):
#     amount = forms.IntegerField(label="purchase amount:", min_value = 1)
#     addrX = forms.CharField(max_length=10, required=False,label="deleivery address X:")   
#     addrY = forms.CharField(max_length=10, required=False, label="deleivery address X:") 
#     upsID = forms.CharField(max_length=10, required=False, label="UPS_ID(optional):") 

#     class Meta:
#         model = UserProfile
#         fields = ['addrX','addrY','upsID']