namespace CodeFirst.Migrations
{
    using System;
    using System.Data.Entity.Migrations;
    
    public partial class InitialCreate : DbMigration
    {
        public override void Up()
        {
            CreateTable(
                "dbo.Managers",
                c => new
                    {
                        ManagerCode = c.String(nullable: false, maxLength: 128),
                        Name = c.String(nullable: false, maxLength: 20),
                        DepartmentId = c.Int(nullable: false),
                    })
                .PrimaryKey(t => t.ManagerCode)
                .ForeignKey("dbo.Departments", t => t.DepartmentId, cascadeDelete: true)
                .Index(t => t.DepartmentId);
            
            CreateTable(
                "dbo.Collaborators",
                c => new
                    {
                        CollaboratorId = c.Int(nullable: false, identity: true),
                        ManagerCode = c.String(maxLength: 128),
                        Name = c.String(nullable: false),
                        DepartmentId = c.Int(nullable: false),
                    })
                .PrimaryKey(t => t.CollaboratorId)
                .ForeignKey("dbo.Managers", t => t.ManagerCode)
                .ForeignKey("dbo.Departments", t => t.DepartmentId, cascadeDelete: true)
                .Index(t => t.ManagerCode)
                .Index(t => t.DepartmentId);
            
            CreateTable(
                "dbo.Departments",
                c => new
                    {
                        DepartmentId = c.Int(nullable: false, identity: true),
                        Name = c.String(nullable: false),
                    })
                .PrimaryKey(t => t.DepartmentId);
            
        }
        
        public override void Down()
        {
            DropIndex("dbo.Collaborators", new[] { "DepartmentId" });
            DropIndex("dbo.Collaborators", new[] { "ManagerCode" });
            DropIndex("dbo.Managers", new[] { "DepartmentId" });
            DropForeignKey("dbo.Collaborators", "DepartmentId", "dbo.Departments");
            DropForeignKey("dbo.Collaborators", "ManagerCode", "dbo.Managers");
            DropForeignKey("dbo.Managers", "DepartmentId", "dbo.Departments");
            DropTable("dbo.Departments");
            DropTable("dbo.Collaborators");
            DropTable("dbo.Managers");
        }
    }
}
