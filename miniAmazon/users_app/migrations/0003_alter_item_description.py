# Generated by Django 4.1.5 on 2023-04-26 23:20

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('users_app', '0002_remove_item_catalog_item_catalog'),
    ]

    operations = [
        migrations.AlterField(
            model_name='item',
            name='description',
            field=models.CharField(default='None', max_length=100),
        ),
    ]